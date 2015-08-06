
#pragma once

#include <windows.h>
#include "singleton.hpp"
#include "input_class.hpp"
#include "graphics_class.hpp"

class system_class : public singleton<system_class>
{

	input_class* _input;
	graphics_class* _graphics;
	bool _is_full_screen;
	int _width;
	int _height;

public:

	system_class();
	~system_class();
	
	bool initialize(int width, int height, Windows::UI::Core::CoreWindow^ core_window);
	void shutdown();
	bool frame_system();

};

//LRESULT CALLBACK window_event_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
