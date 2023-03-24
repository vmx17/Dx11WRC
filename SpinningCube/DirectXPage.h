#pragma once

#include "DirectXPage.g.h"

#include "Common/DeviceResources.h"
#include "SpinningCubeMain.h"

namespace winrt::SpinningCube::implementation
{
    struct DirectXPage : DirectXPageT<DirectXPage>
    {
        DirectXPage();
        virtual ~DirectXPage();

        void SaveInternalState(winrt::Windows::Foundation::Collections::IPropertySet const& state);
        void LoadInternalState(winrt::Windows::Foundation::Collections::IPropertySet const& state);

        void swapChainPanel_PointerPressed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void swapChainPanel_PointerMoved(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void swapChainPanel_PointerReleased(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);

        void AppBarButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        // XAML low-level rendering event handler.
        //void OnRendering(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args);

        // Window event handlers.
        void OnVisibilityChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args);

        // DisplayInformation event handlers.
        void OnDpiChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args);
        void OnOrientationChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args);
        void OnDisplayContentsInvalidated(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args);

        // Other event handlers.
        void OnCompositionScaleChanged(winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel const& sender, winrt::Windows::Foundation::IInspectable const& args);
        void OnSwapChainPanelSizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);

        // Track our independent input on a background worker thread.
        winrt::Windows::Foundation::IAsyncAction m_inputLoopWorker = nullptr;
        winrt::Microsoft::UI::Input::InputPointerSource m_coreInput;

        // Independent input handling functions.
        void SwapChainPanel_OnPointerPressed(winrt::Microsoft::UI::Input::InputPointerSource const& sender, winrt::Microsoft::UI::Input::PointerEventArgs const& e);
        void SwapChainPanel_OnPointerMoved(winrt::Microsoft::UI::Input::InputPointerSource const& sender, winrt::Microsoft::UI::Input::PointerEventArgs const& e);
        void SwapChainPanel_OnPointerReleased(winrt::Microsoft::UI::Input::InputPointerSource const& sender, winrt::Microsoft::UI::Input::PointerEventArgs const& e);

        // Resources used to render the DirectX content in the XAML page background.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::unique_ptr<SpinningCubeMain> m_main;
        bool m_windowVisible;
    };
}

namespace winrt::SpinningCube::factory_implementation
{
    struct DirectXPage : DirectXPageT<DirectXPage, implementation::DirectXPage>
    {
    };
}
