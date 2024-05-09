include "Premake/extensions.lua"

workspace "CapNCrunchEngineSCHOOL"
	location "."
	architecture "x64"
	startproject "CapNCrunchEngineSCHOOL"

	configurations 
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}
	

include "CapNCrunchEngineSCHOOL"