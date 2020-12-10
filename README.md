Ivy Readme                         {#mainpage}
============

# Ivy Framework 

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

TODO: Logo hier einfügen

Ivy is a modern, minimalistic and efficient 3D framework. 
It can be used as a rendering and ecs backend for your game or simulation. 
Supported platforms are Windows 10 and GNU/Linux with a kernel >= 5.0.
Note that this is my honours project for my bachelors degree in Games Programming at SAE Institute Munich.

## Features

- Multiplatform (Linux & Windows)
- ECS
- Multiple mesh and texture formats
- Easy material and shader pipeline
- Input system, window management, fast logging
- Integrated ImGui
- PBR lighting for directional lights and spot lights
- PBR roughness & metallic workflow with texture support
- Image based lighting
- Cascaded shadow mapping
- Postprocessing pass
- Motion blur & depth of field (todo)
- Atmospheric scattering (disabled by default)
- Skeletal animation (todo)
- API Documentation

# Getting Started

Get started by cloning this repository: `git clone https://github.com/TKscoot/Ivy/`

### Required
- C++17 Compiler
	- GCC >=7 (Linux)
	- MSVC >=15.9 (Windows)
- Premake 5
- OpenGL >= 4.5 capable graphics card

### Dependencies
- OpenGL >= 4.5
- GLAD (included)
- GLFW (included)
- Assimp (included)
- glm (included)
- ImGui (included)
- spdlog (included)
- stb_image (included)
- std17

On Windows Visual Studio 2017 or 2019 are recommended. Compiling on Linux was tested with GCC. LLVM/Clang could work but has not been tested.

## Building Ivy on Windows

### Generating projects and solution
Generate project files with Premake5. Make sure your premake executable is in the root folder of the Ivy repository or in your \%PATH%.
Open a command prompt and navigate to the repo root folder and execute following commands.

For Visual Studio 2017 do:
```bash
.\premake5.exe vs2017
```

For Visual Studio 2019 do:
```bash
.\premake5.exe vs2019
```

Then open the Solution and build it inside Visual Studio.

The .lib file of the Ivy Framework can be found in Build/Bin

The .lib files of the external libraries can be found in projects/Ivy/dependencies

## Building Ivy on Linux

Make sure you have the correct GCC version, premake5 and all dependencies installed.

### Geting Premake

**Every distribution**

Download premake-5.0.0-alpha15-linux.tar.gz from the [Premake
Homepage](https://premake.github.io/download.html)\. Unpack the content to your
Ivy repository folder or any folder in your path.

**Arch Linux**
```bash
sudo pacman -S premake
```

### Generating build files

Information about Premake5 and all supported project file generators can be found [here!](https://github.com/premake/premake-core/wiki/Using-Premake)

To generate a GNU makefile with premake you need to execute following command:

```bash
premake5 gmake
```

To build Ivy just type:

``` bash
make
```

The demo program can be run from Build/Bin/Sandbox/\<configuration\>.

## Usage

To use the framework you have to link against following libraries: `Ivy GLAD ImGui glfw assimp`

Linking only to `Ivy` is coming soon™.

If you want to use premake for your project you can use the "Sandbox" template
project from the premake5.lua file in the root of the repo.
