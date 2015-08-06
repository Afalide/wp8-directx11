
#pragma once

#include "engine/system_class.hpp"

namespace stuff
{

ref class myapp sealed : public Windows::ApplicationModel::Core::IFrameworkView
{

public:

	myapp();

	// IFrameworkView interface

	virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
	virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
	virtual void Load(Platform::String^ entryPoint);
	virtual void Run();
	virtual void Uninitialize();

protected:

	// application lifecycle event handlers

	void on_activated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
	void on_suspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
	void on_resuming(Platform::Object^ sender, Platform::Object^ args);

	// window event handlers.

	void on_window_size_changed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
	void on_visibility_changed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
	void on_window_closed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);

	// displayInformation event handlers.

	void on_dpi_changed(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
	void on_orientation_changed(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
	void on_display_contents_invalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

private:

    bool _window_is_open;
    bool _window_is_visible;
    system_class* _system;
    //Windows::UI::Core::CoreWindow^ _core_window;

};

}
