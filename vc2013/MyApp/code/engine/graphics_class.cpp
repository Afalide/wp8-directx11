
#include <iostream>
#include "../debug.hpp"
#include "graphics_class.hpp"

using namespace stuff;

graphics_class::graphics_class()
    : _d3d(NULL)
    , _color_shader(NULL)
    , _texture_shader(NULL)
    , _camera(NULL)
    , _model(NULL)
{
}

graphics_class::~graphics_class()
{
}

bool
graphics_class::initialize(int width, int height, Windows::UI::Core::CoreWindow^ core_window)
{
    debug::d() << "initializing graphics..." << debug::endl;

    _d3d = new d3d_class;
    _color_shader = new color_shader_class;
    _camera = new camera_class;
    _model = new model_class;
    _texture_shader = new texture_shader_class;

    if
    (!(
        _d3d->initialize(width, height, core_window, 0.1f, 1000.0f)
        //&& _color_shader->initialize(_d3d->get_device())
        && _texture_shader->initialize(_d3d->get_device())
        && _model->initialize(_d3d->get_device(), L"code/engine/textures/tigre.dds")
    ))
    {
        debug::d() << "failed to initialize graphics" << debug::endl;
        return false;
    }

    //camera's default position
    _camera->set_position(0.0f, 0.0f, -10.0f);

    //model's default rotation
    _model->set_rotation_y(0.0f);

    return true;
}

void
graphics_class::shutdown()
{
    debug::d() << "shutting down graphics..." << debug::endl;

    if(NULL != _d3d)
    {
        _d3d->shutdown();
        delete _d3d;
        _d3d = NULL;
    }

    if(NULL != _color_shader)
    {
        _color_shader->shutdown();
        delete _color_shader;
        _color_shader = NULL;
    }

    if(nullptr != _texture_shader)
    {
        _texture_shader->shutdown();
        delete _texture_shader;
        _texture_shader = NULL;
    }

    if(NULL != _model)
    {
        _model->shutdown();
        delete _model;
        _model = NULL;
    }

    if(NULL != _camera)
    {
        delete _camera;
        _camera = NULL;
    }
}

bool
graphics_class::frame_graphics()
{
    _model->animate();

    if(! _render())
        return false;

    return true;
}

bool
graphics_class::_render()
{
    // Clear the buffers to begin the scene.
    _d3d->begin_scene(0.3f, 0.3f, 0.6f, 1.0f);

    //enable trasparency
    _d3d->enable_alpha_blending();

    // Generate the view matrix based on the camera's position.
    _camera->render();

    // Get the world, view, and projection matrices from the camera and d3d objects.
    DirectX::XMFLOAT4X4 matrix_view  = _camera->get_view_matrix();
    DirectX::XMFLOAT4X4 matrix_world = _d3d->get_world_matrix();
    DirectX::XMFLOAT4X4 matrix_proj  = _d3d->get_projection_matrix();
    DirectX::XMFLOAT4X4 matrix_model = _model->get_model_matrix();

    // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
    _model->render(_d3d->get_device_context());

    // Render the model using the color shader.
    //bool result = _color_shader->render(_d3d->get_device_context()
    //                                  , _model->get_index_count()
    //                                  , &matrix_world
    //                                  , &matrix_view
    //                                  , &matrix_proj);
    //if(!result)
    //{
    //    debug::d() << "failed to render color shader" << debug::endl;
    //    return false;
    //}

    //render the model using the texture shader
    
    bool result = _texture_shader->render(_d3d->get_device_context()
                                        , _model->get_index_count()
                                        , &matrix_world
                                        , &matrix_model
                                        , &matrix_view
                                        , &matrix_proj
                                        , _model->get_texture_resource_view());
    if(!result)
    {
        debug::d() << "failed to render texture shader" << debug::endl;
        return false;
    }

    // Present the rendered scene to the screen.
    _d3d->end_scene();

    return true;
}
