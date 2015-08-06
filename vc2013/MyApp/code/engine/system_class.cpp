
#include <iostream>
#include "system_class.hpp"
#include "../debug.hpp"

using namespace stuff;

register_singleton(system_class)

system_class::system_class()
    : _graphics(NULL)
    , _input(NULL)
    , _is_full_screen(false)
    , _width(0)
    , _height(0)
{
}

system_class::~system_class()
{

}

bool
system_class::initialize(int width, int height, Windows::UI::Core::CoreWindow^ core_window)
{
    debug::d() << "initializing system..." << debug::endl;

    _graphics = new graphics_class;
    _input = new input_class;

    if
    (!(
        _graphics->initialize(width, height, core_window)
        && _input->initialize()
    ))
    {
        debug::d() << "failed to initialize system" << debug::endl;
        return false;
    }

    return true;
}

void
system_class::shutdown()
{
    debug::d() << "shutting down system..." << debug::endl;

    //shutdown_window();

    if(_graphics)
    {
        _graphics->shutdown();
        delete _graphics;
        _graphics = NULL;
    }

    if(_input)
    {
        _input->shutdown();
        delete _input;
        _input = NULL;
    }
}

bool
system_class::frame_system()
{
    if(_input->is_key_down(VK_ESCAPE))
        return false;

    if(! _graphics->frame_graphics())
        return false;

    return true;
}
