
#pragma once

namespace stuff
{

ref class myapp_factory sealed
	: public Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ 
		CreateView();
};

}
