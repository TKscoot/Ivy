-- Helper Functions
require "export-compile-commands"
require "deploy"

function useIvyLib()
	-- The library's public headers
	includedirs "projects/Ivy"
	
	-- We link against a library that's in the same workspace, so we can just
	-- use the project name - premake is really smart and will handle everything for us.
	links "Ivy"
end

-- The name of your workspace will be used, for example, to name the Visual Studio .sln file generated by Premake.
workspace "Ivy"
	-- We set the location of the files Premake will generate
	
    --configuration { "linux", "gmake" }
    --buildoptions { "-j 4" } 
	-- We indicate that all the projects are C++ only
	language "C++"
	-- Language standard should be C++17
	cppdialect "C++17"
	
	-- We will compile for x86_64. You can change this to x86 for 32 bit builds.
	architecture "x86_64"
	
	-- Configurations are often used to store some compiler / linker settings together.
	configurations { "Debug", "Release" }
	
	-- We use filters to set options, a new feature of Premake5.
	
	-- We now only set settings for the Debug configuration
	filter { "configurations:Debug" }
		-- We want debug symbols in our debug config
		symbols "On"
		flags{"MultiProcessorCompile"} 
	
	-- We now only set settings for Release
	filter { "configurations:Release" }
		-- Release should be optimized
		optimize "On"
		flags{"MultiProcessorCompile"} 
	
	-- Reset the filter for other settings
	filter { }
	
	-- Here we use some "tokens" (the things between %{ ... }). They will be replaced by Premake
	-- The path is relative to *this* folder
	targetdir ("Build/Bin/%{prj.name}/%{cfg.longname}")
	objdir ("Build/Obj/%{prj.name}/%{cfg.longname}")

	
project "Buildfiles"  -- selects workspace scope, if you haven't seen it before
	kind "None"
    files { "*.lua" } -- workspace level files

project "GLAD"
	
	location "projects/Ivy/dependencies/GLAD"
	
	targetdir ("projects/Ivy/dependencies/GLAD/Lib")
	objdir ("projects/Ivy/dependencies/GLAD/Lib")
	
	kind "StaticLib"
	
	language "C"

	files
	{
		"projects/Ivy/dependencies/GLAD/include/glad/glad.h",
		"projects/Ivy/dependencies/GLAD/include/KHR/khrplatform.h",
		"projects/Ivy/dependencies/GLAD/src/glad.c"
	}
	
	includedirs "projects/Ivy/dependencies/GLAD/include"
	
	filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
		
    filter{ }	

project "ImGui"
	
	kind "StaticLib"
	
	location "projects/Ivy/dependencies/ImGui"

	targetdir ("projects/Ivy/dependencies/ImGui/Lib")
	objdir	  ("projects/Ivy/dependencies/ImGui/Lib")

	files "projects/Ivy/dependencies/ImGui/**"
	includedirs "projects/Ivy/dependencies/ImGui/"

	includedirs
	{
		"projects/Ivy/dependencies/GLFW/Include",
		"projects/Ivy/dependencies/GLAD/include"
	}
	
	libdirs
	{
		"projects/Ivy/dependencies/GLFW/Lib",
		"projects/Ivy/dependencies/GLAD/Lib"
	}
	
    links "GLAD"

    filter "system:linux"
        links "glfw"
    filter {}

    filter "system:windows"
        links "GLFW3"
    filter {}

	filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
		
    filter{ }	

-- The core engine, the static library
project "Ivy"
	
	location "projects/Ivy"
	
	kind "StaticLib"
	
	includedirs
	{
		"projects/Ivy/source",
		"projects/Ivy/dependencies/GLFW/Include",
		"projects/Ivy/dependencies/GLAD/include",
		"projects/Ivy/dependencies/spdlog/include",
		"projects/Ivy/dependencies/glm",
		"projects/Ivy/dependencies/stb_image",
		"projects/Ivy/dependencies/assimp/include",
		"projects/Ivy/dependencies/ImGui",
		"projects/Ivy/dependencies/taskflow",
		"projects/Ivy/dependencies/OpenAl/include",
		"projects/Ivy/dependencies/libsndfile/include",
		"projects/Ivy/dependencies/effolkronium",
		"projects/Ivy/dependencies/FMOD/inc",
		"projects/Ivy/dependencies/irrKlang/include",
		"projects/Ivy/dependencies/MagicEnum"
	}
	
	libdirs
	{
		"projects/Ivy/dependencies/GLFW/Lib",
		"projects/Ivy/dependencies/GLAD/Lib",
		"projects/Ivy/dependencies/assimp/lib",
		"projects/Ivy/dependencies/ImGui/lib",
		"projects/Ivy/dependencies/OpenAl/lib",
		"projects/Ivy/dependencies/libsndfile/lib",
		"projects/Ivy/dependencies/FMOD/lib/x64",
		"projects/Ivy/dependencies/irrKlang/lib"
	}
	
    links 
	{
		"GLAD",
		"ImGui"
	}

    filter "system:linux"
        links "glfw"
        links "assimp"
		links "libsndfile"
    filter {}

	filter{ "system:windows", "configurations:Debug"}
        links "GLFW3"
		links "assimp-vc141-mt"
		links "OpenAL32.lib"
		links "libsndfile-1.lib"
		links "fmod_vc.lib"
		links "irrKlang.lib"
    filter {}
	
	filter{ "system:windows", "configurations:Release"}
        links "GLFW3"
		links "assimp-vc141-mt"
		links "OpenAL32.lib"
		links "libsndfile-1.lib"
		links "fmod_vc.lib"
		links "irrKlang.lib"
    filter {}

    -- We specify where the source files are.
	-- It would be better to separate header files in a folder and sources
	-- in another, but for our simple project we will put everything in the same place.
	-- Note: ** means recurse in subdirectories, so it will get all the files in ExampleLib/
	pchheader "ivypch.h"
    pchsource "projects/Ivy/source/ivypch.cpp"
	files "projects/Ivy/source/**"
	

-- The windowed sandbox app
project "Sandbox"

	location "projects/Sandbox"
	
	kind "ConsoleApp"
	files "projects/Sandbox/source/**"

	-- We also need the headers
	includedirs 
	{
		"projects/Ivy/source",
		"projects/Ivy/dependencies/GLFW/Include",
		"projects/Ivy/dependencies/GLAD/include",
		"projects/Ivy/dependencies/spdlog/include",
		"projects/Ivy/dependencies/glm",
		"projects/Ivy/dependencies/stb_image",
		"projects/Ivy/dependencies/assimp/include",
		"projects/Ivy/dependencies/ImGui",
		"projects/Ivy/dependencies/taskflow",
		"projects/Ivy/dependencies/OpenAl/include",
		"projects/Ivy/dependencies/libsndfile/include",
		"projects/Ivy/dependencies/effolkronium",
		"projects/Ivy/dependencies/FMOD/inc",
		"projects/Ivy/dependencies/irrKlang/include",
		"projects/Ivy/dependencies/MagicEnum"
	}
	

	libdirs
	{
		"projects/Ivy/dependencies/GLAD/Lib",
		"projects/Ivy/dependencies/ImGui/Lib",
		"projects/Ivy/dependencies/OpenAl/lib",
		"projects/Ivy/dependencies/libsndfile/lib",
		"projects/Ivy/dependencies/FMOD/lib/x64",
		"projects/Ivy/dependencies/irrKlang/lib"
	}

	LibraryDir = {}
	filter "configurations:Debug"
	LibraryDir["Assimp"] = "projects/Ivy/dependencies/assimp/lib/assimp-vc141-mt.lib"
	LibraryDir["Ivy"] = "Build/Bin/Ivy/Debug/Ivy.lib"
	--filter "configurations:Release"
	--LibraryDir["Assimp"] = "projects/Ivy/dependencies/assimp/lib/assimp-vc141-mt.lib"
	--LibraryDir["Ivy"] = "Build/Bin/Ivy/Release/Ivy.lib"
	filter{}
	LibraryDir["GLFW"] = "projects/Ivy/dependencies/GLFW/lib/glfw3.lib"
	LibraryDir["GLAD"] = "projects/Ivy/dependencies/GLAD/Lib/GLAD.lib"
	LibraryDir["ImGui"] = "projects/Ivy/dependencies/ImGui/Lib/ImGui.lib"

    filter "system:windows"
        libdirs
        {
			"projects/Ivy/dependencies/assimp/lib",
            "projects/Ivy/dependencies/GLFW/lib"

        }

		


		postbuildcommands{
			"cd \"$(SolutionDir)\"",
			"call \"$(SolutionDir)MergeLibs.bat\" /OUT:IvyMerged.lib %{LibraryDir.Ivy} %{LibraryDir.Assimp} %{LibraryDir.GLFW} %{LibraryDir.GLAD} %{LibraryDir.ImGui}",
			-- copying shaders to build folder
			"xcopy \"$(SolutionDir)projects\\Sandbox\\shaders\\*.*\" \"$(TargetDir)shaders\\\" /y /e /i /f"
		}
    filter{}

    links "Ivy"
    links "GLAD"
	links "ImGui"
	filter{ "system:windows", "configurations:Debug"}
        links "GLFW3"
		links "assimp-vc141-mt"
		links "OpenAL32.lib"
		links "libsndfile-1.lib"
		links "fmod_vc.lib"
		links "irrKlang.lib"
    filter {}
	
	filter{ "system:windows", "configurations:Release"}
        links "GLFW3"
		links "assimp-vc141-mt"		
		links "OpenAL32.lib"
		links "libsndfile-1.lib"
		links "fmod_vc.lib"
		links "irrKlang.lib"
    filter {}

    filter "system:linux"
        links "dl"
        links "glfw"
        links "assimp"
        links "pthread"
		links "openal"
		links "libsndfile"
    filter {}


