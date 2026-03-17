workspace "TerminalStory Project"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "TerminalCore"
	location "TerminalCore"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/".. outputdir .."/%{prj.name}")
	objdir ("bin-int/".. outputdir .."/%{prj.name}")

	-- 设置 UTF-8 编码
	buildoptions { "/utf-8" }

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"TC_PLATFORM_WINDOWS",
			"TC_BUILD_DLL",
		}

		postbuildcommands
		{
		   "xcopy /Y /I \"$(OutDir)TerminalCore.dll\" \"$(SolutionDir)bin\\%{outputdir}\\TerminalStory Project\\\" > nul"
		}
	
	filter "configurations:Debug"
		defines "TS_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "TS_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "TS_DIST"
		optimize "On"

project "TerminalStory Project"
	location "TerminalStory Project"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/".. outputdir .."/%{prj.name}")
	objdir ("bin-int/".. outputdir .."/%{prj.name}")

	-- 设置 UTF-8 编码
	buildoptions { "/utf-8" }

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"TerminalCore/src"
	}

	links
	{
		"TerminalCore"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"TC_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "TS_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "TS_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "TS_DIST"
		optimize "On"