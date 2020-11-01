workspace "Seagull"
    architecture "x64"
    startproject "Sea"

    configurations
    {
        "Debug", 
		"Release",
		"Dist"
    }

-- Debug-windows-x64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
IncludeDir = { }
IncludeDir["spdlog"] = "Seagull Core/vendor/spdlog/include"

group "Dependencies"

group ""

project "Seagull Core"
    location "Seagull Core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    -- bin/Debug-windows-x64/Seagull Core
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    -- bin-int/Debug-windows-x64/Seagull Core
    objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- precompile header
    pchheader "sgpch.h"
    pchsource "Seagull Core/src/sgpch.cpp"

    -- include files
    files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

    -- define macros
    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    -- include directories
    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDir.spdlog}"
    }

    -- link libraries

filter "system:windows"
    systemversion "latest"
    defines
    {
        "SG_PLATFORM_WINDOWS",
        "SG_GRAPHIC_API_DX12"
    }

filter "configurations:Debug"
    defines 
    {
        "SG_DEBUG",
        "SG_ENABLE_ASSERT"
    }
    runtime "Debug"
    symbols "on"

filter "configurations:Release"
    defines "SG_RELEASE"
    runtime "Release"
    optimize "on"

filter "configurations:Dist"
    defines "SG_DIST"
    runtime "Release"
    optimize "on"

    
project "Sea"
    location "Sea"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    -- bin/Debug-windows-x64/Seagull Core
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    -- bin-int/Debug-windows-x64/Seagull Core
    objdir    ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- include files
    files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

    -- include directories
    includedirs
    {
        "Seagull Core/src",
        "%{IncludeDir.spdlog}"
    }

    -- link libraries
    links
    {
        "Seagull Core"
    }

filter "system:windows"
    systemversion "latest"
    defines "SG_PLATFORM_WINDOWS"

filter "configurations:Debug"
    defines "SG_DEBUG"
    runtime "Debug"
    symbols "on"
    postbuildcommands { "editbin/subsystem:console $(OutDir)$(ProjectName).exe" } -- enable console

filter "configurations:Release"
    defines "SG_RELEASE"
    runtime "Release"
    optimize "on"

filter "configurations:Dist"
    defines "SG_DIST"
    runtime "Release"
    optimize "on"
