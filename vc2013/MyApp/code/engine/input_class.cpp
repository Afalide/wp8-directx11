
#include <iostream> 
#include <windows.h>

#include "input_class.hpp"
#include "../debug.hpp"

using namespace stuff;

input_class::input_class()
{
}

input_class::~input_class()
{
}

bool
input_class::initialize()
{
	debug::d() << "initializing input..." << debug::endl;

	for(int i = 0; i < 256; i++)
		_keys[i] = false;

	return true;
}

void
input_class::shutdown()
{
	debug::d() << "shutting down input..." << debug::endl;
}

void
input_class::set_key_down(unsigned int key)
{
	_keys[key] = true;
}

void
input_class::set_key_up(unsigned int key)
{
	_keys[key] = false;
}

bool
input_class::is_key_down(unsigned int key)
{
	return _keys[key];
}
