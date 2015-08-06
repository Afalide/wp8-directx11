
#include <sstream>
#include <windows.h>
#include <cstdlib>
#include <cstring>
#include "debug.hpp"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

void 
stuff::enable_memory_leaks_detection()
{
#ifdef _DEBUG

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#else

    stuff::debug::d() << "this application is not compiled with _DEBUG flag" << std::endl;

#endif
}

char* stuff::debug::endl = "\r\n";

void
stuff::debugstr(const char* str)
{
    size_t origsize = strlen(str) + 1;
    const size_t newsize = 100;
    size_t wrote = 0;
    wchar_t wcstring[newsize];
    mbstowcs_s(&wrote, wcstring, origsize, str, _TRUNCATE);
    ::OutputDebugString(wcstring);
}

stuff::debug
stuff::debug::d()
{
    debug d;
    return d;
}

stuff::debug&
stuff::debug::operator<<(const char* data)
{
    stuff::debugstr(data);
    return *this;
}

stuff::debug&
stuff::debug::operator<<(const char data)
{
    std::stringstream buf;
    buf << data;
    std::string bufstr = buf.str();
    stuff::debugstr(bufstr.c_str());
    return *this;
}

stuff::debug&
stuff::debug::operator<<(const int& data)
{
    std::stringstream buf;
    buf << data;
    std::string bufstr = buf.str();
    stuff::debugstr(bufstr.c_str());
    return *this;
}

stuff::debug&
stuff::debug::operator<<(const unsigned int& data)
{
    std::stringstream buf;
    buf << data;
    std::string bufstr = buf.str();
    stuff::debugstr(bufstr.c_str());
    return *this;
}

stuff::debug&
stuff::debug::operator<<(const float& data)
{
    std::stringstream buf;
    buf << data;
    std::string bufstr = buf.str();
    stuff::debugstr(bufstr.c_str());
    return *this;
}

stuff::debug&
stuff::debug::operator<<(const void* address)
{
    std::stringstream buf;
    buf << address;
    std::string bufstr = buf.str();
    stuff::debugstr(bufstr.c_str());
    return *this;
}

stuff::debug&
stuff::debug::operator<<(const std::string& data)
{
    std::stringstream buf;
    buf << data;
    std::string bufstr = buf.str();
    stuff::debugstr(bufstr.c_str());
    return *this;
}

stuff::debug&
stuff::debug::operator<<(const HRESULT& data)
{
    unsigned int code = static_cast<unsigned int>(data);
    return this->operator<< (code);
}

stuff::debug& 
stuff::debug::operator<< (const wchar_t* data)
{
    ::OutputDebugString(data);
    return *this;
}
