
#pragma once

#include <D3D11.h>

class texture_class
{
private:

    ID3D11Resource*             _texture_resource;
    ID3D11ShaderResourceView*   _texture_resource_view;

public:

    texture_class();
    ~texture_class();

    bool initialize(ID3D11Device* device, const wchar_t* file_path);
    void shutdown();

    ID3D11ShaderResourceView* get_texture_resource_view();
};
