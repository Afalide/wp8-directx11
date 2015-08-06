
#include "myapp_factory.hpp"
#include "myapp.hpp"

Windows::ApplicationModel::Core::IFrameworkView^
stuff::myapp_factory::CreateView()
{
	return ref new myapp;
}
