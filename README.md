Ivy Readme                         {#mainpage}
============

# Ivy Framework 

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Documentation Status](https://readthedocs.org/projects/ansicolortags/badge/?version=latest)](https://ivy3d.org)

Ivy is a modern, minimalistic and efficient 3D framework. 
It can be used as a rendering and ecs backend for your game or simulation. 
Supported platforms are Windows 10 and GNU/Linux with a kernel >= 5.0.
Note that this is my honours project for my bachelors degree in Games Programming at SAE Institute Munich.



\htmlonly
<video src="Final_Trailer.mp4" width="830" height="auto" controls>
</video>
\endhtmlonly

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
- Motion blur & depth of field
- Atmospheric scattering (disabled by default)
- Skeletal animation
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

### Code sample

```cpp
#include <Ivy.h>

using namespace Ivy;

int main()
{
	// Creating the Engine object
	Ptr<Engine> engine = CreatePtr<Engine>();

	// Initializing Ivy and creating a window
	engine->Initialize(1280, 720, "Sample App");

	// Getting the pointer to the scene
	Ptr<Scene> scene = Scene::GetScene();

	// Setting up skybox with 6 textures for the cubetexture
	Vector<String> skyboxTextures =
	{
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg"
	};

	// Telling the scene to use these textures as skybox
	scene->SetSkybox(skyboxTextures);

	// Changing the direction of the default directional light
	scene->SetDirectionalLightDirection(Vec3(-2.0f, 4.0f, -1.0f));
	
	// Adding a pointlight to our scene
	scene->AddPointLight(
		Vec3(-1.0f, 1.0f, 0.0f),
		1.0f,
		0.09f,
		0.032,
		Vec3(0.05f, 0.05f, 0.05f),
		Vec3(1.0f, 0.0f, 0.0f),
		Vec3(1.0f, 1.0f, 1.0f));
		
	// Adding an entity to our scene
	Ptr<Entity> sampleEntity = scene->CreateEntity();
	
	// Attaching a Mesh component to our sample entity
	sampleEntity->AddComponent(CreatePtr<Mesh>(sampleEntity.get(), "assets/models/Cerberus.FBX"));
	
	// Getting the default Material component from our entity
	Ptr<Material> sampleMat = towerEntity->GetFirstComponentOfType<Material>();
	
	// Assigning normal, metallic and roughness PBR textures to the material
	sampleMat->LoadTexture("assets/textures/Cerberus/Cerberus_N.tga", Material::TextureMapType::NORMAL);
	sampleMat->LoadTexture("assets/textures/Cerberus/Cerberus_M.tga", Material::TextureMapType::METALLIC);
	sampleMat->LoadTexture("assets/textures/Cerberus/Cerberus_R.tga", Material::TextureMapType::ROUGHNESS);
	
	
	// Game loop while the engine shouldn't terminate
	while(!engine->ShouldTerminate())
	{
		// Exit our program when the escape key is pressed down
		if(Input::IsKeyDown(KeyCode::Escape))
		{
			exit(0);
		}

		// Telling Ivy to begin the new frame
		engine->NewFrame();

	}
}
```
