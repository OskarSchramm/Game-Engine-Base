workspace "CapNCrunchEngineSCHOOL"
	location "."
	architecture "x64"

	configurations 
	{
		"Debug",
		"Release",
		"Distribution"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "CapNCrunchEngineSCHOOL"
	location "CapNCrunchEngineSCHOOL"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	debugdir (outputdir)

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.hlsl",
		"**.hlsli"
	}

	includedirs {
		"external/"
	}

	filter "configurations:Debug"
		defines "_DEBUG"
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
	filter "configurations:Distribution"
		defines "_DIST"
		runtime "Release"
		optimize "on"

	filter("files:**_PS.hlsl")
		shadermodel("5.0")
		shadertype("Pixel")

   filter("files:**_VS.hlsl")
		shadermodel("5.0")
		shadertype("Vertex")