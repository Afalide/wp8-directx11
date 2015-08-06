
#pragma once

#include <windows.h>
#include "d3d_class.hpp"
#include "color_shader_class.hpp"
#include "texture_shader_class.hpp"
#include "model_class.hpp"
#include "camera_class.hpp"

class graphics_class
{

    d3d_class*             _d3d;
    color_shader_class*    _color_shader;
    texture_shader_class*  _texture_shader;
    model_class*           _model;
    camera_class*          _camera;

public:

    graphics_class();
    ~graphics_class();

    bool initialize(int width
                  , int height
                  , Windows::UI::Core::CoreWindow^ core_window);

    void shutdown();
    bool frame_graphics();

    void rotate_triangle_y(float y);

private:

    bool _render();

};
