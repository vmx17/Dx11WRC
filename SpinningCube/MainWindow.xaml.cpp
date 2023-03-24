#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "DirectXPage.g.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml::Media::Animation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SpinningCube::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();

        RootFrame().Navigate(xaml_typename<winrt::SpinningCube::DirectXPage>(), nullptr, SuppressNavigationTransitionInfo());
    }
}
