# Dx11WRC
An interop example C++ DirectX and XAML in C# as Windows Runtime Component

# What is for
This repository is for ask questions.

# Content of this repository.
The code is for VisualStudio 2022.
There are two solution files.
1) `SpinningCube.sln` is a solution to draw spinning cube in C++ and its projection project in C#.
This is not a WRC project but a C++/XAML application. If you run it, a window open and a spinning cube shown.
and get `*.nupkg` also.
2) `SpinningCubeTest.sln` is a consuming splution in C#.

# What problem is...
I just could not refer `page` control build in `SpinningCube.sln` from `SpinningCubeTest.sln`.

The `*.idl` code is:
_DirectXPage.idl_
```
namespace SpinningCube
{
    [default_interface]
    runtimeclass DirectXPage : Microsoft.UI.Xaml.Controls.Page
    {
        DirectXPage();
    }
}
```
referring it from `SpinningCubeTest.sln` something like:
```
public MainWindow()
{
	this.InitializeComponent();
	RootFrame.Navigate(typeof(SpinningCube.DirectXPage), null);
}
```
but I cannot get `SpinningCube.DirectXPage`.