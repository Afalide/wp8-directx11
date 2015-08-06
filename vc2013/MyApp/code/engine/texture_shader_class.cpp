

#include <cstdio>
#include <iostream>
#include "texture_shader_class.hpp"
#include "../debug.hpp"

using namespace stuff;

texture_shader_class::texture_shader_class()
    : _vertex_shader(nullptr)
    , _pixel_shader(nullptr)
    , _input_layout(NULL)
    , _matrix_cbuffer(NULL)
    , _sampler_state(nullptr)
{
}

texture_shader_class::~texture_shader_class()
{
}

bool
texture_shader_class::initialize(ID3D11Device* device)
{
    debug::d() << "initializing texture shader..." << debug::endl;

    if(! _initialize_shader(device
        , "code/engine/shaders/texture_shader_vertex.bin"
        , "code/engine/shaders/texture_shader_pixel.bin"))
    {
        debug::d() << "failed to initialize texture shader (internal)" << debug::endl;
        return false;
    }

    return true;
}

void
texture_shader_class::shutdown()
{
    debug::d() << "shutting down texture shader..." << debug::endl;
    _shutdown_shader();
}

bool
texture_shader_class::render(ID3D11DeviceContext* device_context
                           , int index_count
                           , DirectX::XMFLOAT4X4* matrix_world
                           , DirectX::XMFLOAT4X4* matrix_model
                           , DirectX::XMFLOAT4X4* matrix_view
                           , DirectX::XMFLOAT4X4* matrix_projection
                           , ID3D11ShaderResourceView* texture_resource_view)
{
    //setup all the shaders parameters before rendering

    if(! _set_shader_parameters(device_context
                              , matrix_world
                              , matrix_model
                              , matrix_view
                              , matrix_projection
                              , texture_resource_view))
    {
        debug::d() << "failed to set shader parameters" << debug::endl;
        return false;
    }

    //draw the triangle

    _render_shader(device_context, index_count);

    return true;
}

bool
texture_shader_class::_initialize_shader(ID3D11Device* device
                                       , const char* vso_file
                                       , const char* pso_file)
{
    debug::d() << "initializing texture shader (internal)..." << debug::endl;

#pragma region load precompiled shader objects

    //shaders are precompiled using the fxc.exe tool from the
    //windows sdk (see the build_shaders.bat script)

    HRESULT result;

    //load precompiled vertex shader

    size_t vertex_shader_bytes_length;
    char* vertex_shader_bytes = _read_bytes_from_file(vso_file, &vertex_shader_bytes_length);
    if(NULL == vertex_shader_bytes)
    {
        debug::d() << "failed to get bytes from " << vso_file << debug::endl;
        return false;
    }
    result = device->CreateVertexShader(vertex_shader_bytes
                                      , vertex_shader_bytes_length
                                      , NULL
                                      , &_vertex_shader);
    if(FAILED(result))
    {
        debug::d() << "failed to create vertex shader pointer"
            << " (code " << result << ")" << debug::endl;
        delete[] vertex_shader_bytes;
        return false;
    }

    //load precompiled pixel shader

    size_t pixel_shader_bytes_length;
    char* pixel_shader_bytes = _read_bytes_from_file(pso_file, &pixel_shader_bytes_length);
    if(NULL == pixel_shader_bytes)
    {
        debug::d() << "failed to get bytes from " << pso_file << debug::endl;
        delete[] vertex_shader_bytes;
        return false;
    }
    result = device->CreatePixelShader(pixel_shader_bytes
                                     , pixel_shader_bytes_length
                                     , NULL
                                     , &_pixel_shader);

    delete[] pixel_shader_bytes;

    if(FAILED(result))
    {
        debug::d() << "failed to create pixel shader pointer" << debug::endl;
        delete[] vertex_shader_bytes;
        return false;
    }

#pragma endregion

#pragma region setup the vertex shader layout

    //the layout describes how CPU buffers maps the shader's buffers
    //for each value in these buffers, a shader pass wil be done
    //typical example: the vertex buffer contains vectors (positions).
    //for each position, a vertex will be drawn

    //POSITION is a reserved keyword, it identifies the value as a vertex position buffer
    //TEXCOORD is a reserved keyword, it identifies the value as a UV texture coordinate

    //for the position value, the w component is set by the shader to 1, so only 
    //3 components from the cpu are required (x,y,z) 
    //DirectX::XMFLOAT3 -> position (float4, see above) we set x,y,z.  w is set by the shader
    //DirectX::XMFLOAT2 -> uvcoord (float2)   

    D3D11_INPUT_ELEMENT_DESC layout_infos[2];

    //set the layout for the POSITION float4
    layout_infos[0].SemanticName = "POSITION";
    layout_infos[0].SemanticIndex = 0;
    layout_infos[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; //x,y,z
    layout_infos[0].InputSlot = 0;
    layout_infos[0].AlignedByteOffset = 0;
    layout_infos[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout_infos[0].InstanceDataStepRate = 0;

    //set the layout for the TEXCOORD float2
    layout_infos[1].SemanticName = "TEXCOORD";
    layout_infos[1].SemanticIndex = 0;
    layout_infos[1].Format = DXGI_FORMAT_R32G32_FLOAT; //u,v
    layout_infos[1].InputSlot = 0;
    layout_infos[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    layout_infos[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout_infos[1].InstanceDataStepRate = 0;

    //remember: sizeof on a static array (declared with [] )
    //returns the total size in bytes of the array
    //sizeof on a pointer (*) will always return the size of
    //an int (or the size of a long for x64)

    int count = sizeof(layout_infos) / sizeof(D3D11_INPUT_ELEMENT_DESC);
    result = device->CreateInputLayout(layout_infos
                                     , count
                                     , vertex_shader_bytes
                                     , vertex_shader_bytes_length
                                     , &_input_layout);
    if(FAILED(result))
    {
        debug::d() << "failed to create vertex input layout, "
            << "check if the layout infos corresponds to the "
            << "shader code" << debug::endl;
        delete[] vertex_shader_bytes;
        return false;
    }

    delete[] vertex_shader_bytes;

#pragma endregion

#pragma region load the matrix buffer (constant buffer)

    //the constant buffer is a set of values which are the same
    //for each vertex of the array when the shader is computing.
    //this is the same as "uniforms" in opengl vertex shaders.
    //the reserved HLSL name is 'cbuffer', see it in the shader.vs.
    //we put later into that cbuffer the 3 matrices: model, view and projection
    //since for the model (ie: the color triangle), these matrices
    //have the same values for each vertex

    D3D11_BUFFER_DESC matrix_cbuffer_infos;
    matrix_cbuffer_infos.Usage = D3D11_USAGE_DYNAMIC;
    matrix_cbuffer_infos.ByteWidth = sizeof(matrix_buffer_type);
    matrix_cbuffer_infos.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrix_cbuffer_infos.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrix_cbuffer_infos.MiscFlags = 0;
    matrix_cbuffer_infos.StructureByteStride = 0;

    //create the cbuffer. note: it has no values, they are passed during
    //the rendering phase

    result = device->CreateBuffer(&matrix_cbuffer_infos, NULL, &_matrix_cbuffer);
    if(FAILED(result))
    {
        debug::d() << " failed to create matrix cbuffer" << debug::endl;
        return false;
    }

#pragma endregion

#pragma region create the sampler

    //the sampler is a pixel shader object which finds the correct
    //texture color for a given UV coordinate

    //describe the sampler

    D3D11_SAMPLER_DESC sampler_infos;
    
    sampler_infos.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;   //"nearest"
    //sampler_infos.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  //"linear"
    
    sampler_infos.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //clamp values on range 0.0 to 1.0
    sampler_infos.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_infos.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_infos.MipLODBias = 0.0f;
    sampler_infos.MaxAnisotropy = 1;
    sampler_infos.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampler_infos.BorderColor[0] = 0;
    sampler_infos.BorderColor[1] = 0;
    sampler_infos.BorderColor[2] = 0;
    sampler_infos.BorderColor[3] = 0;
    sampler_infos.MinLOD = 0;
    sampler_infos.MaxLOD = D3D11_FLOAT32_MAX;

    //create the sampler

    result = device->CreateSamplerState(&sampler_infos, &_sampler_state);
    if(FAILED(result))
    {
        debug::d() << "failed to create sampler state" << debug::endl;
        return false;
    }

#pragma endregion

    return true;
}

void
texture_shader_class::_shutdown_shader()
{
    debug::d() << "shutting down texture shader (internal)" << debug::endl;

    if(NULL != _vertex_shader)
    {
        _vertex_shader->Release();
        _vertex_shader = NULL;
    }

    if(NULL != _pixel_shader)
    {
        _pixel_shader->Release();
        _pixel_shader = NULL;
    }

    if(NULL != _input_layout)
    {
        _input_layout->Release();
        _input_layout = NULL;
    }

    if(NULL != _matrix_cbuffer)
    {
        _matrix_cbuffer->Release();
        _matrix_cbuffer = NULL;
    }

    if(nullptr != _sampler_state)
    {
        _sampler_state->Release();
        _sampler_state = NULL;
    }
}

bool
texture_shader_class::_set_shader_parameters(ID3D11DeviceContext* device_context
                                           , DirectX::XMFLOAT4X4* matrix_world
                                           , DirectX::XMFLOAT4X4* matrix_model
                                           , DirectX::XMFLOAT4X4* matrix_view
                                           , DirectX::XMFLOAT4X4* matrix_projection
                                           , ID3D11ShaderResourceView* texture_resource_view)
{
    //debug::d() << "setting shader parameters..." << debug::endl;

    HRESULT result;

    //before sending the matrixes to the cbuffer, transpose them

    DirectX::XMMATRIX xm_mworld;
    DirectX::XMMATRIX xm_mmodel;
    DirectX::XMMATRIX xm_mview;
    DirectX::XMMATRIX xm_mproj;

    xm_mworld = DirectX::XMLoadFloat4x4(matrix_world);
    xm_mmodel = DirectX::XMLoadFloat4x4(matrix_model);
    xm_mview = DirectX::XMLoadFloat4x4(matrix_view);
    xm_mproj = DirectX::XMLoadFloat4x4(matrix_projection);

    xm_mworld = DirectX::XMMatrixTranspose(xm_mworld);
    xm_mmodel = DirectX::XMMatrixTranspose(xm_mmodel);
    xm_mview = DirectX::XMMatrixTranspose(xm_mview);
    xm_mproj = DirectX::XMMatrixTranspose(xm_mproj);

    //lock the matrices cbuffer

    D3D11_MAPPED_SUBRESOURCE mapped_resource;

    result = device_context->Map(_matrix_cbuffer
                                 , 0
                                 , D3D11_MAP_WRITE_DISCARD
                                 , 0
                                 , &mapped_resource);
    if(FAILED(result))
    {
        debug::d() << "failed to get a GPU lock for the cbuffer" << debug::endl;
        return false;
    }

    //get a pointer to the data inside the cbuffer

    matrix_buffer_type* matrix_data;
    matrix_data = static_cast<matrix_buffer_type*>(mapped_resource.pData);

    //copy the matrices inside the cbuffer

    DirectX::XMStoreFloat4x4(&(matrix_data->world), xm_mworld);
    DirectX::XMStoreFloat4x4(&(matrix_data->model), xm_mmodel);
    DirectX::XMStoreFloat4x4(&(matrix_data->view), xm_mview);
    DirectX::XMStoreFloat4x4(&(matrix_data->projection), xm_mproj);

    //unlock the cbuffer

    device_context->Unmap(_matrix_cbuffer, 0);

    //tell the vertex shader to use the updated buffer

    device_context->VSSetConstantBuffers(0, 1, &_matrix_cbuffer);

    //tell the pixel shader to use the .dds texture

    device_context->PSSetShaderResources(0, 1, &texture_resource_view);

    return true;
}

void
texture_shader_class::_render_shader(ID3D11DeviceContext* device_context, int index_count)
{
    // toggle the input layout

    device_context->IASetInputLayout(_input_layout);

    //toggle the vertex and pixel shaders

    device_context->VSSetShader(_vertex_shader, NULL, 0);
    device_context->PSSetShader(_pixel_shader, NULL, 0);

    // set the sampler state in the pixel shader
    device_context->PSSetSamplers(0, 1, &_sampler_state);

    //draw!

    device_context->DrawIndexed(index_count, 0, 0);
}

char*
texture_shader_class::_read_bytes_from_file(const char* file_name, size_t* buffer_size)
{
    std::FILE* f = NULL;
    fopen_s(&f, file_name, "rb");
    if(NULL == f)
    {
        debug::d() << "failed to open file " << file_name << debug::endl;
        return NULL;
    }
    std::fseek(f, 0, SEEK_END);
    *buffer_size = std::ftell(f);
    char* buffer = new char[*buffer_size];
    std::fseek(f, 0, SEEK_SET);
    if(1 != std::fread(buffer, *buffer_size, 1, f))
    {
        debug::d() << "failed to read file " << file_name << debug::endl;
        delete[] buffer;
        return NULL;
    }
    debug::d() << "read " << *buffer_size << " bytes from "
        << file_name << debug::endl;
    return buffer;
}