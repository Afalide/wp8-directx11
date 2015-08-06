
#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <d3d11.h>

class texture_shader_class
{
    struct matrix_buffer_type
    {
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4X4 model;
        DirectX::XMFLOAT4X4 view;
        DirectX::XMFLOAT4X4 projection;
    };

    ID3D11VertexShader*       _vertex_shader;
    ID3D11PixelShader*        _pixel_shader;
    ID3D11InputLayout*        _input_layout;
    ID3D11Buffer*             _matrix_cbuffer;
    ID3D11SamplerState*       _sampler_state;

public:

    texture_shader_class();
    ~texture_shader_class();

    bool initialize(ID3D11Device*);
    void shutdown();
    bool render(ID3D11DeviceContext* device_context
                , int index_count
                , DirectX::XMFLOAT4X4* matrix_world
                , DirectX::XMFLOAT4X4* matrix_model
                , DirectX::XMFLOAT4X4* matrix_view
                , DirectX::XMFLOAT4X4* matrix_projection
                , ID3D11ShaderResourceView* texture_resource_view);

private:

    bool _initialize_shader(ID3D11Device*, const char*, const char*);
    void _shutdown_shader();
    bool _set_shader_parameters(ID3D11DeviceContext* device_context
                                , DirectX::XMFLOAT4X4* matrix_world
                                , DirectX::XMFLOAT4X4* matrix_model
                                , DirectX::XMFLOAT4X4* matrix_view
                                , DirectX::XMFLOAT4X4* matrix_projection
                                , ID3D11ShaderResourceView* texture_resource_view);
    void _render_shader(ID3D11DeviceContext*, int);
    char* _read_bytes_from_file(const char* file_name, size_t* buffer_size);
};

