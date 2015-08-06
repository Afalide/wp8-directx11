
#include "debug.hpp"
#include "myapp.hpp"
#include "myapp_factory.hpp"

//[Platform::MTAThread]

int main(Platform::Array<Platform::String^>^ args)
{
    stuff::enable_memory_leaks_detection();
	stuff::debugstr("main begin\n");
	auto myappfactory = ref new stuff::myapp_factory;
	Windows::ApplicationModel::Core::CoreApplication::Run(myappfactory);
	stuff::debugstr("main end\n");
	return 0;
}
