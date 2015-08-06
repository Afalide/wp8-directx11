
#include "texture_class.hpp"
#include "loaddswrap.hpp"
#include "../debug.hpp"

using namespace stuff;

texture_class::texture_class()
    : _texture_resource(nullptr)
    , _texture_resource_view(nullptr)
{
}

texture_class::~texture_class()
{

}

bool
texture_class::initialize(ID3D11Device* device, const wchar_t* file_path)
{
    debug::d() << "initializing texture..." << debug::endl;

    HRESULT result;

    result = loadds_from_file(device, file_path, &_texture_resource, &_texture_resource_view);
    if(FAILED(result))
    {
        debug::d() << "failed to load texture dds from file"
            << " (code " << result << ")" << debug::endl;
        return false;
    }

    return true;
}

void texture_class::shutdown()
{
    if(nullptr != _texture_resource)
    {
        _texture_resource->Release();
        _texture_resource = nullptr;
    }

    if(nullptr != _texture_resource_view)
    {
        _texture_resource_view->Release();
        _texture_resource_view = nullptr;
    }
}

ID3D11ShaderResourceView* 
texture_class::get_texture_resource_view()
{
    return _texture_resource_view;
}