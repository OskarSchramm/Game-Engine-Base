project "CapNCrunchEngineSCHOOL"
	location "."
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"

	buildConfig = "%{cfg.buildcfg}/"           
	solutionDir = "%{wks.location}/"
	prjName = "%{prj.name}"           
	outputDir = "$(SolutionDir)bin"
	
	targetdir (outputDir) 							 --ouput dir  
    objdir(solutionDir .. "temp/" .. prjName)  		 --intermediate dir
	targetname("%{prj.name}_%{cfg.buildcfg}")   	 --target name			  
	debugdir(outputDir)

	flags{
		"MultiProcessorCompile",
		"FatalCompileWarnings"
	}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.hlsl",
		"**.hlsli",
	}

	warnings "High"

	shadermodel("5.0")
	local shaderDir = (solutionDir .. "bin/shaders/")
	os.mkdir("../bin/shaders/")

	print("SolutionDir: " .. solutionDir)
	print("ShaderDir: " .. shaderDir)

	filter("files:**.hlsl")
		shadermodel("5.0")
		shaderobjectfileoutput(shaderDir .. "%{file.basename}"..".cso")

	filter("files:**_PS.hlsl")
		shadermodel("5.0")
		shadertype("Pixel")
		shaderobjectfileoutput(shaderDir .. "%{file.basename}"..".cso")

   filter("files:**_VS.hlsl")
		shadermodel("5.0")
		shadertype("Vertex")
		shaderobjectfileoutput(shaderDir .. "%{file.basename}"..".cso")

		filter("files:**_CS.hlsl")
		shadermodel("5.0")
		shadertype("Compute")
		shaderobjectfileoutput(shaderDir .. "%{file.basename}"..".cso")

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"
		
	filter "configurations:Distribution"
		defines "NDEBUG"
		runtime "Release"
		optimize "on"