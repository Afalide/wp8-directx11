
#include <iostream>
#include "model_class.hpp"
#include "../debug.hpp"
#include "matrix_tools.hpp"
#include <cmath>

using namespace stuff;

model_class::model_class()
    : _index_count(0)
    , _vertex_count(0)
    , _vertex_buffer(NULL)
    , _index_buffer(NULL)
    , _texture(nullptr)
    , _vec_pos(nullptr)
    , _vec_scale(nullptr)
    , _vec_rotation(nullptr)
    , _animation_index(0)
{
}

model_class::~model_class()
{
}

bool
model_class::initialize(ID3D11Device* device, const wchar_t* texture_path)
{
    debug::d() << "initializing model..." << debug::endl;

    _texture = new texture_class;

    _vec_pos = new DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    _vec_scale = new DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    _vec_rotation = new DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    if
    (!(
           _initialize_buffers(device)
        && _texture->initialize(device, texture_path)
    ))
    {
        debug::d() << "failed to initialize model" << debug::endl;
        return false;
    }

    return true;
}

void
model_class::shutdown()
{
    debug::d() << "shutting down model..." << debug::endl;
    _shutdown_buffers();

    if(nullptr != _texture)
    {
        _texture->shutdown();
        delete _texture;
        _texture = nullptr;
    }
}

void
model_class::render(ID3D11DeviceContext* device_context)
{
    _render_buffers(device_context);
}

bool
model_class::_initialize_buffers(ID3D11Device* device)
{
    debug::d() << "initializing model buffers..." << debug::endl;
    HRESULT result;

#pragma region create vertices and indices

    //create a triangle, vertices are in clockwise order

    _vertex_count = 3;
    _index_count = 3;


    vertex_type* vertices = new vertex_type[_vertex_count];
    unsigned int* indices = new unsigned int[_index_count];

    vertices[0].position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f); //bottom left
    //vertices[0].color = DirectX::XMFLOAT4(0.9f, 0.1f, 0.1f, 1.0f); //this vertex is red
    vertices[0].uvcoord = DirectX::XMFLOAT2(0.0f, 1.0f); //the bottom left UV

    vertices[1].position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f); //top
    //vertices[1].color = DirectX::XMFLOAT4(0.1f, 0.1f, 0.9f, 1.0f); //this vertex is blue
    vertices[1].uvcoord = DirectX::XMFLOAT2(0.5f, 0.0f); //the middle top UV


    vertices[2].position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f); //bottom right
    //vertices[2].color = DirectX::XMFLOAT4(0.1f, 0.9f, 0.1f, 1.0f); //this vertex is green
    vertices[2].uvcoord = DirectX::XMFLOAT2(1.0f, 1.0f); //the bottom right UV

    indices[0] = 0; //bootom left
    indices[1] = 1; //top
    indices[2] = 2; //bottom right

#pragma endregion

#pragma region create buffers

    //describe the vertex buffer

    D3D11_BUFFER_DESC vertex_buffer_infos;
    vertex_buffer_infos.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_infos.ByteWidth = sizeof(vertex_type) * _vertex_count;
    vertex_buffer_infos.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_infos.CPUAccessFlags = 0;
    vertex_buffer_infos.StructureByteStride = 0;
    vertex_buffer_infos.MiscFlags = 0;

    //describe the vertex buffer data

    D3D11_SUBRESOURCE_DATA vertex_data_infos;
    vertex_data_infos.pSysMem = vertices;
    vertex_data_infos.SysMemPitch = 0;
    vertex_data_infos.SysMemSlicePitch = 0;

    //create the vertex buffer

    result = device->CreateBuffer(&vertex_buffer_infos, &vertex_data_infos, &_vertex_buffer);
    if(FAILED(result))
    {
        debug::d() << "failed to create the vertex buffer" << debug::endl;
        return false;
    }

    //describe the index buffer

    D3D11_BUFFER_DESC index_buffer_infos;
    index_buffer_infos.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_infos.ByteWidth = sizeof(unsigned int) * _index_count;
    index_buffer_infos.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_infos.CPUAccessFlags = 0;
    index_buffer_infos.StructureByteStride = 0;
    index_buffer_infos.MiscFlags = 0;

    //describe the index buffer data

    D3D11_SUBRESOURCE_DATA index_data_infos;
    index_data_infos.pSysMem = indices;
    index_data_infos.SysMemPitch = 0;
    index_data_infos.SysMemSlicePitch = 0;

    //create the index buffer

    result = device->CreateBuffer(&index_buffer_infos, &index_data_infos, &_index_buffer);
    if(FAILED(result))
    {
        debug::d() << "failed to create the index buffer" << debug::endl;
        return false;
    }

    //in-ram values are no longer needed

    delete[] vertices;
    delete[] indices;
    vertices = NULL;
    indices = NULL;

#pragma endregion

    return true;
}

void
model_class::_shutdown_buffers()
{
    debug::d() << "shutting down model buffers..." << debug::endl;

    if(_index_buffer)
    {
        _index_buffer->Release();
        _index_buffer = NULL;
    }

    if(_vertex_buffer)
    {
        _vertex_buffer->Release();
        _vertex_buffer = NULL;
    }
}

void
model_class::_render_buffers(ID3D11DeviceContext* device_context)
{
    unsigned int stride = sizeof(vertex_type);
    unsigned int offset = 0;

    //activate this class vertex and index buffer in the gpu

    device_context->IASetVertexBuffers(0, 1, &_vertex_buffer, &stride, &offset);
    device_context->IASetIndexBuffer(_index_buffer, DXGI_FORMAT_R32_UINT, 0);

    //tell directx our vertex buffer contains triangles

    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

ID3D11ShaderResourceView* 
model_class::get_texture_resource_view()
{
    return _texture->get_texture_resource_view();
}

int
model_class::get_index_count()
{
    return _index_count;
}

int
model_class::get_vertex_count()
{
    return _vertex_count;
}

void
model_class::set_rotation_y(float rotation)
{
    if(rotation > 360.0f)
        rotation = 0;

    _vec_rotation->y = rotation;
}

float
model_class::get_rotation_y()
{
    return _vec_rotation->y;
}

DirectX::XMFLOAT4X4
model_class::get_model_matrix()
{
    //the scale matrix

    DirectX::XMMATRIX xmm_scale = DirectX::XMMatrixIdentity();

    //the translation matrix

    DirectX::XMMATRIX xmm_translation = DirectX::XMMatrixIdentity();

    //the rotation matrix

    DirectX::XMFLOAT4X4 m_rotation = matrix_tools::make_rotation_matrix(0, get_rotation_y(), 0);
    DirectX::XMMATRIX xmm_rotation = DirectX::XMLoadFloat4x4(&m_rotation);

    //geometry pipeline:  some_vertex * mworld * mmodel * mview * mperspective
    //with model matrix =  mscale * mtranslation * mrotation

    DirectX::XMMATRIX scale_x_translation             = DirectX::XMMatrixMultiply(xmm_scale, xmm_translation);
    DirectX::XMMATRIX scale_x_translation_x_rotation  = DirectX::XMMatrixMultiply(scale_x_translation, xmm_rotation);
 
    DirectX::XMFLOAT4X4 ret;
    DirectX::XMStoreFloat4x4(&ret, scale_x_translation_x_rotation);
    return ret;
}

void
model_class::animate()
{
    //rotation = cos(frame/20)*75
    //http://www.intmath.com/functions-and-graphs/graphs-using-svg.php?function1=cos(x%2F40)*45&function2=&xMin=-160&xMax=160&xGrid=10&yMin=-80&yMax=80&yGrid=10&gwidth=550&gheight=350&picture1input=setBorder(0)%3Bfontstyle%3D"normal"%3BinitPicture(-160%2C160%2C-80%2C80)%3Baxes(10%2C10%2C+"labels"%2C+10)%3Bstroke+%3D+fnACol%3Bplot(fn1)%3Bstroke+%3D+fnBCol%3Bplot(fn2)%3B

    float rotation = ::cos(static_cast<float>(_animation_index++) /20) *75;
    set_rotation_y(rotation);
}
