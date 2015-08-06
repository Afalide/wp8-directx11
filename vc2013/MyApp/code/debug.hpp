
#pragma once

#include <string>
#include <winerror.h> //HRESULT

namespace stuff
{

void debugstr(const char*);
void enable_memory_leaks_detection();

class debug
{
public:

	static char* endl;

    debug& operator<< (const char*);
	debug& operator<< (const char);
    debug& operator<< (const int&);
    debug& operator<< (const unsigned int&);
	debug& operator<< (const float&);
    debug& operator<< (const void*);
    debug& operator<< (const std::string&);
    debug& operator<< (const HRESULT&);
    debug& operator<< (const wchar_t*);

	static debug d();
};

}
