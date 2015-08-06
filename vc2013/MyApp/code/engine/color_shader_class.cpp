
#include <cstdio>
#include <iostream>
#include "color_shader_class.hpp"
#include "../debug.hpp"

using namespace stuff;

color_shader_class::color_shader_class()
    : _vertex_shader(NULL)
    , _pixel_shader(NULL)
    , _input_layout(NULL)
    , _matrix_cbuffer(NULL)
{
}

color_shader_class::~color_shader_class()
{
}

bool
color_shader_class::initialize(ID3D11Device* device)
{
    debug::d() << "initializing color shader..." << debug::endl;

    if(! _initialize_shader(device
                          , "code/engine/shaders/color_shader_vertex.bin"
                          , "code/engine/shaders/color_shader_pixel.bin"))
    {
        debug::d() << "failed to initialize color shader (internal)" << debug::endl;
        return false;
    }

    return true;
}

void
color_shader_class::shutdown()
{
    debug::d() << "shutting down color shader..." << debug::endl;
    _shutdown_shader();
}

bool
color_shader_class::render(ID3D11DeviceContext* device_context
                          , int index_count
                          , DirectX::XMFLOAT4X4* matrix_world
                          , DirectX::XMFLOAT4X4* matrix_view
                          , DirectX::XMFLOAT4X4* matrix_projection)
{
    //setup all the shaders parameters before rendering

    if(! _set_shader_parameters(device_context
                               , matrix_world
                               , matrix_view
                               , matrix_projection))
    {
        debug::d() << "failed to set shader parameters" << debug::endl;
        return false;
    }

    //draw the triangle

    _render_shader(device_context, index_count);

    return true;
}

bool
color_shader_class::_initialize_shader(ID3D11Device* device
                                      , const char* vso_file
                                      , const char* pso_file)
{
    debug::d() << "initializing color shader (internal)..." << debug::endl;

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

    //the layout describes how CPU variables maps the vertex shader's variables
    //for the position value, the w component is set by the shader to 1, so only 
    //3 components from the cpu are required (x,y,z) 
    //DirectX::XMFLOAT3 -> position (float4, see above) we set x,y,z; w is set by the shader
    //DirectX::XMFLOAT4 -> color (float4)

    D3D11_INPUT_ELEMENT_DESC layout_infos[2];

    //set the layout for the SOME_POSITION float4
    layout_infos[0].SemanticName = "POSITION";
    layout_infos[0].SemanticIndex = 0;
    layout_infos[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; //x,y,z
    layout_infos[0].InputSlot = 0;
    layout_infos[0].AlignedByteOffset = 0;
    layout_infos[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout_infos[0].InstanceDataStepRate = 0;

    //set the layout for the SOME_COLOR float4
    layout_infos[1].SemanticName = "COLOR";
    layout_infos[1].SemanticIndex = 0;
    layout_infos[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //x,y,z,w
    layout_infos[1].InputSlot = 0;
    layout_infos[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    layout_infos[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout_infos[1].InstanceDataStepRate = 0;

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

    //create the cbuffer. note: it has no values: they are passed during
    //the rendering phase

    result = device->CreateBuffer(&matrix_cbuffer_infos, NULL, &_matrix_cbuffer);
    if(FAILED(result))
    {
        debug::d() << " failed to create matrix cbuffer" << debug::endl;
        return false;
    }

#pragma endregion

    return true;
}

void
color_shader_class::_shutdown_shader()
{
    debug::d() << "shutting down color shader (internal)" << debug::endl;

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
}

bool
color_shader_class::_set_shader_parameters(ID3D11DeviceContext* device_context
                                         , DirectX::XMFLOAT4X4* matrix_world
                                         , DirectX::XMFLOAT4X4* matrix_view
                                         , DirectX::XMFLOAT4X4* matrix_projection)
{
    //debug::d() << "setting shader parameters..." << debug::endl;

    HRESULT result;

    //before sending the matrixes to the cbuffer, transpose them

    DirectX::XMMATRIX xm_mworld;
    DirectX::XMMATRIX xm_mview;
    DirectX::XMMATRIX xm_mproj;

    xm_mworld = DirectX::XMLoadFloat4x4(matrix_world);
    xm_mview = DirectX::XMLoadFloat4x4(matrix_view);
    xm_mproj = DirectX::XMLoadFloat4x4(matrix_projection);

    xm_mworld = DirectX::XMMatrixTranspose(xm_mworld);
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
    DirectX::XMStoreFloat4x4(&(matrix_data->view), xm_mview);
    DirectX::XMStoreFloat4x4(&(matrix_data->projection), xm_mproj);

    //unlock the cbuffer

    device_context->Unmap(_matrix_cbuffer, 0);

    //tell the vertex shader to use the updated buffer

    device_context->VSSetConstantBuffers(0, 1, &_matrix_cbuffer);

    return true;
}

void
color_shader_class::_render_shader(ID3D11DeviceContext* device_context, int index_count)
{
    // toggle the input layout

    device_context->IASetInputLayout(_input_layout);

    //toggle the vertex and oixel shaders

    device_context->VSSetShader(_vertex_shader, NULL, 0);
    device_context->PSSetShader(_pixel_shader, NULL, 0);

    //draw!

    device_context->DrawIndexed(index_count, 0, 0);
}

char*
color_shader_class::_read_bytes_from_file(const char* file_name, size_t* buffer_size)
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