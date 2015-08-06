#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "texture_class.hpp"

class model_class
{
    struct vertex_type
    {
        DirectX::XMFLOAT3 position;
        //DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 uvcoord;
    };

    int                    _vertex_count;
    int                    _index_count;
    ID3D11Buffer*          _vertex_buffer;
    ID3D11Buffer*          _index_buffer;
    texture_class*         _texture;
    DirectX::XMFLOAT3*     _vec_pos;
    DirectX::XMFLOAT3*     _vec_scale;
    DirectX::XMFLOAT3*     _vec_rotation;
    int                    _animation_index;

public:

    model_class();
    ~model_class();

    bool initialize(ID3D11Device* device, const wchar_t* texture_path);
    void shutdown();
    void render(ID3D11DeviceContext* device_context);
    int get_index_count();
    int get_vertex_count();
    ID3D11ShaderResourceView* get_texture_resource_view();
    void set_rotation_y(float rotation);
    float get_rotation_y();
    DirectX::XMFLOAT4X4 get_model_matrix();
    void animate();

private:

    bool _initialize_buffers(ID3D11Device* device);
    void _shutdown_buffers();
    void _render_buffers(ID3D11DeviceContext* device_context);
};

