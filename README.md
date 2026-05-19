# Project Setup

To build and run this project, it is recommended to use **Visual Studio 2022** with the following workloads installed:

- Game Development with C++
- Desktop Development with C++

These can be installed through the Visual Studio Installer.

## DXC Compiler Setup

Before building the project, open:

```
Scripts/CompileShader.cmake
```
Find 
```
set(DXC_EXECUTABLE "C:/DXC/bin/x64/dxc.exe")
```
And than replace the path with dxc path on you computer

