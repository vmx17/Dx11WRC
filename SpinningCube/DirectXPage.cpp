#include "pch.h"
#include "DirectXPage.h"
#if __has_include("DirectXPage.g.cpp")
#include "DirectXPage.g.cpp"
#endif

#define USE_INDEPENDENT_INPUT_SOURCE 1

using namespace winrt;
using namespace Microsoft::UI::Input;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Controls::Primitives;
using namespace Microsoft::UI::Xaml::Data;
using namespace Microsoft::UI::Xaml::Input;
using namespace Microsoft::UI::Xaml::Media;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace concurrency;

namespace winrt::SpinningCube::implementation
{
    DirectXPage::DirectXPage() :
        m_windowVisible(true),
        m_coreInput(nullptr)
    {
        InitializeComponent();

        // Register event handlers for page lifecycle.
        //Windows::UI::Core::CoreWindow window = Window::Current().CoreWindow();

        //window.VisibilityChanged(TypedEventHandler<Windows::UI::Core::CoreWindow, VisibilityChangedEventArgs>(this, &DirectXPage::OnVisibilityChanged));

        //DisplayInformation currentDisplayInformation = DisplayInformation::GetForCurrentView();

        //currentDisplayInformation.DpiChanged(TypedEventHandler<DisplayInformation, IInspectable>(this, &DirectXPage::OnDpiChanged));

        //currentDisplayInformation.OrientationChanged(TypedEventHandler<DisplayInformation, IInspectable>(this, &DirectXPage::OnOrientationChanged));

        //DisplayInformation::DisplayContentsInvalidated(TypedEventHandler<DisplayInformation, IInspectable>(this, &DirectXPage::OnDisplayContentsInvalidated));

        swapChainPanel().CompositionScaleChanged(TypedEventHandler<SwapChainPanel, IInspectable>(this, &DirectXPage::OnCompositionScaleChanged));

        swapChainPanel().SizeChanged(SizeChangedEventHandler(this, &DirectXPage::OnSwapChainPanelSizeChanged));

        // At this point we have access to the device. 
        // We can create the device-dependent resources.
        m_deviceResources = std::make_shared<DX::DeviceResources>();
        m_deviceResources->SetSwapChainPanel(swapChainPanel());

#if USE_INDEPENDENT_INPUT_SOURCE
        // Register our SwapChainPanel to get independent input pointer events
        auto workItemHandler = WorkItemHandler([this](IAsyncAction)
            {
                // The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread it's created on.
                InputPointerSourceDeviceKinds deviceKind = (InputPointerSourceDeviceKinds)
                    ((uint32_t)Microsoft::UI::Input::InputPointerSourceDeviceKinds::Mouse |
                        (uint32_t)Microsoft::UI::Input::InputPointerSourceDeviceKinds::Touch |
                        (uint32_t)Microsoft::UI::Input::InputPointerSourceDeviceKinds::Pen);
                m_coreInput = swapChainPanel().CreateCoreIndependentInputSource(
                    deviceKind
                );

                // Register for pointer events, which will be raised on the background thread.
                m_coreInput.PointerPressed({ this, &DirectXPage::SwapChainPanel_OnPointerPressed });
                m_coreInput.PointerMoved({ this, &DirectXPage::SwapChainPanel_OnPointerMoved });
                m_coreInput.PointerReleased({ this, &DirectXPage::SwapChainPanel_OnPointerReleased });
            });

        // Run task on a dedicated high priority background thread.
        m_inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
#else
        swapChainPanel().PointerPressed({ this, &DirectXPage::swapChainPanel_PointerPressed });
        swapChainPanel().PointerMoved({ this, &DirectXPage::swapChainPanel_PointerMoved });
        swapChainPanel().PointerReleased({ this, &DirectXPage::swapChainPanel_PointerReleased });
#endif
        m_main = std::unique_ptr<SpinningCubeMain>(new SpinningCubeMain(m_deviceResources));
        m_main->StartRenderLoop();
    }

    DirectXPage::~DirectXPage()
    {
        // Stop rendering and processing events on destruction.
        m_main->StopRenderLoop();
    }

    // Saves the current state of the app for suspend and terminate events.
    void DirectXPage::SaveInternalState(IPropertySet const& state)
    {
        critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_deviceResources->Trim();

        // Stop rendering when the app is suspended.
        m_main->StopRenderLoop();

        // Put code to save app state here.
    }

    // Loads the current state of the app for resume events.
    void DirectXPage::LoadInternalState(IPropertySet const& state)
    {
        // Put code to load app state here.

        // Start rendering when the app is resumed.
        m_main->StartRenderLoop();
    }

    // Window event handlers.

    void DirectXPage::OnVisibilityChanged(Windows::UI::Core::CoreWindow const& sender, VisibilityChangedEventArgs const& args)
    {
        m_windowVisible = args.Visible();
        if (m_windowVisible)
        {
            m_main->StartRenderLoop();
        }
        else
        {
            m_main->StopRenderLoop();
        }
    }

    // DisplayInformation event handlers.

    void DirectXPage::OnDpiChanged(DisplayInformation const& sender, IInspectable const& args)
    {
        critical_section::scoped_lock lock(m_main->GetCriticalSection());
        // Note: The value for LogicalDpi retrieved here may not match the effective DPI of the app
        // if it is being scaled for high resolution devices. Once the DPI is set on DeviceResources,
        // you should always retrieve it using the GetDpi method.
        // See DeviceResources.cpp for more details.
        m_deviceResources->SetDpi(sender.LogicalDpi());
        m_main->CreateWindowSizeDependentResources();
    }

    void DirectXPage::OnOrientationChanged(DisplayInformation const& sender, IInspectable const& args)
    {
        critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_deviceResources->SetCurrentOrientation(sender.CurrentOrientation());
        m_main->CreateWindowSizeDependentResources();
    }

    void DirectXPage::OnDisplayContentsInvalidated(DisplayInformation const& sender, IInspectable const& args)
    {
        critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_deviceResources->ValidateDevice();
    }

    // Called when the app bar button is clicked.
    void DirectXPage::AppBarButton_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        // Use the app bar if it is appropriate for your app. Design the app bar, 
        // then fill in event handlers (like this one).
    }

    void DirectXPage::SwapChainPanel_OnPointerPressed(InputPointerSource const& sender, Microsoft::UI::Input::PointerEventArgs const& e)
    {
        // When the pointer is pressed begin tracking the pointer movement.
        m_main->StartTracking();
    }

    void DirectXPage::SwapChainPanel_OnPointerMoved(InputPointerSource const& sender, Microsoft::UI::Input::PointerEventArgs const& e)
    {
        // Update the pointer tracking code.
        if (m_main->IsTracking())
        {
            m_main->TrackingUpdate(e.CurrentPoint().Position().X);
        }
    }

    void DirectXPage::SwapChainPanel_OnPointerReleased(InputPointerSource const& sender, Microsoft::UI::Input::PointerEventArgs const& e)
    {
        // Stop tracking pointer movement when the pointer is released.
        m_main->StopTracking();
    }

    void DirectXPage::OnCompositionScaleChanged(SwapChainPanel const& sender, IInspectable const& args)
    {
        critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_deviceResources->SetCompositionScale(sender.CompositionScaleX(), sender.CompositionScaleY());
        m_main->CreateWindowSizeDependentResources();
    }

    void DirectXPage::OnSwapChainPanelSizeChanged(IInspectable const& sender, SizeChangedEventArgs const& e)
    {
        critical_section::scoped_lock lock(m_main->GetCriticalSection());
        m_deviceResources->SetLogicalSize(e.NewSize());
        m_main->CreateWindowSizeDependentResources();
    }

    void DirectXPage::swapChainPanel_PointerPressed(IInspectable const& sender, PointerRoutedEventArgs const& e)
    {
        m_main->StartTracking();
    }

    void DirectXPage::swapChainPanel_PointerMoved(IInspectable const& sender, PointerRoutedEventArgs const& e)
    {
        // Update the pointer tracking code.
        if (m_main->IsTracking())
        {
            m_main->TrackingUpdate(e.GetCurrentPoint(swapChainPanel()).Position().X);
        }
    }

    void DirectXPage::swapChainPanel_PointerReleased(IInspectable const& sender, PointerRoutedEventArgs const& e)
    {
        m_main->StopTracking();
    }
}
