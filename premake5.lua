workspace "CVulkan"
    configurations { "Debug", "Release" }
    platforms { "Win32", "Win64" }

    defaultplatform "Win64"
    
    -- Platform Settings
    filter "platforms:*32*"
        architecture "x86"
    
    filter "platforms:*64*"
        architecture "x86_64"

    filter {}

project "CVulkan"
    location "build"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"
    debugdir "./resources"
    language "C"
    kind "ConsoleApp"

    files {
        "src/**.h",
        "src/**.c",
    }
    includedirs {
        "third-party/stb",
    }

-- Windows Settings
filter "platforms:Win*"
    defines {}
    includedirs {
        os.getenv("VULKAN_SDK").."/Include",
    }
    libdirs {
        os.getenv("VULKAN_SDK").."/Lib",
    }
    links {
        "xinput.lib",
        "vulkan-1.lib",
    }
    buildoptions {}
    linkoptions {}
    prebuildcommands {}
    postbuildcommands {}

filter { "platforms:Win*", "configurations:*Debug*" }
    optimize "Off"
    symbols "On"

filter { "platforms:Win*", "configurations:not *Debug*" }
    optimize "On"

filter {}


