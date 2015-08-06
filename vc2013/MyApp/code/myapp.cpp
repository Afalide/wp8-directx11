
#include "myapp.hpp"
#include "debug.hpp"

#include <iostream>

#include <thread> //test, remove this asap

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

stuff::myapp::myapp()
    : _window_is_open(false)
    , _window_is_visible(false)
    , _system(nullptr)
{
}

// The first method called when the IFrameworkView is being created.
void
stuff::myapp::Initialize(CoreApplicationView^ applicationView)
{
    stuff::debugstr("initialize\n");

    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>
        (this, &myapp::on_activated);

    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>
        (this, &myapp::on_suspending);

    CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>
        (this, &myapp::on_resuming);
}

// Called when the CoreWindow object is created (or re-created).
void
stuff::myapp::SetWindow(CoreWindow^ window)
{
    debug::d() << "setwindow" << debug::endl;

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>
        (this, &myapp::on_visibility_changed);

    window->Closed +=
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>
        (this, &myapp::on_window_closed);

    window->SizeChanged +=
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>
        (this, &myapp::on_window_size_changed);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation^, Object^>
        (this, &myapp::on_display_contents_invalidated);

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>
        (this, &myapp::on_dpi_changed);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>
        (this, &myapp::on_orientation_changed);

    // Disable all pointer visual feedback for better performance when touching.
    // This is not supported on Windows Phone applications.
    //auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
    //pointerVisualizationSettings->IsContactFeedbackEnabled = false; 
    //pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;
}

// Initializes scene resources, or loads a previously saved app state.
void
stuff::myapp::Load(Platform::String^ entryPoint)
{
    debug::d() << "load" << debug::endl;
}

// This method is called after the window becomes active.
void stuff::myapp::Run()
{
    debug::d() << "run\n";

    Windows::UI::Core::CoreWindow^ core_window = Windows::UI::Core::CoreWindow::GetForCurrentThread();

    _system = new system_class;
    bool initialized = _system->initialize(static_cast<int>(core_window->Bounds.Width)
                                         , static_cast<int>(core_window->Bounds.Height)
                                         , core_window);
    if(! initialized)
        debug::d() << "system not initialized, frames will not be rendered" << debug::endl;

    while(_window_is_open)
    {
        if(initialized && _window_is_visible)
            _system->frame_system();

        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent); //do not wait if no events
        //CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending); //wait for events
    }
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void stuff::myapp::Uninitialize()
{
    if(nullptr != _system)
    {
        _system->shutdown();
        delete _system;
        _system = nullptr;
    }

    debug::d() << "uninitialize" << debug::endl;
}

// Application lifecycle event handlers.

void stuff::myapp::on_activated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
    debug::d() << "on_activated" << debug::endl;
    CoreWindow::GetForCurrentThread()->Activate();
    _window_is_open = true;
}

void stuff::myapp::on_suspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
    debug::d() << "on_suspending" << debug::endl;

    // Save app state asynchronously after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations. Be
    // aware that a deferral may not be held indefinitely. After about five seconds,
    // the app will be forced to exit.


}

void
stuff::myapp::on_resuming(Platform::Object^ sender, Platform::Object^ args)
{
    debug::d() << "on_resuming" << debug::endl;

    // Restore any data or state that was unloaded on suspend. By default, data
    // and state are persisted when resuming from suspend. Note that this event
    // does not occur if the app was previously terminated.


}

// Window event handlers.

void stuff::myapp::on_visibility_changed(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
    debug::d() << "on_visibility_changed (now "
               << ((args->Visible) ? "visible" : "hidden")
               << ")" << debug::endl;

    _window_is_visible = args->Visible;
}

void stuff::myapp::on_window_closed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
    debug::d() << "on_window_closed" << debug::endl;
    _window_is_open = false;
}

void stuff::myapp::on_window_size_changed(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
    debug::d() << "on_window_size_changed" << debug::endl;
}

// DisplayInformation event handlers.

void stuff::myapp::on_display_contents_invalidated(DisplayInformation^ sender, Object^ args)
{
    debug::d() << "on_display_contents_invalidated" << debug::endl;
}

void stuff::myapp::on_dpi_changed(DisplayInformation^ sender, Object^ args)
{
    debug::d() << "on_dpi_changed" << debug::endl;
}

void stuff::myapp::on_orientation_changed(DisplayInformation^ sender, Object^ args)
{
    debug::d() << "on_orientation_changed" << debug::endl;
}
