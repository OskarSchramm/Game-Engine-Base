workspace "CapNCrunchEngineSCHOOL"
	location "."
	architecture "x64"

	configurations 
	{
		"Debug",
		"Release",
		"Distribution"
	}

output = "%{cfg.buildcfg}"

project "CapNCrunchEngineSCHOOL"
	location "CapNCrunchEngineSCHOOL"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. output)
	objdir("temp/" .. output)
	targetname("%{prj.name}_%{cfg.buildcfg}")
	debugdir ("bin/")

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

	filter("files:**_PS.hlsl")
		shadermodel("5.0")
		shadertype("Pixel")
		shaderobjectfileoutput("../bin/shaders/".."%{file.basename}"..".cso")

   filter("files:**_VS.hlsl")
		shadermodel("5.0")
		shadertype("Vertex")
		shaderobjectfileoutput("../bin/shaders/".."%{file.basename}"..".cso")

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