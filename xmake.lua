cpp_version = "c++17"
openal_include_directory_path = "C:/Program Files (x86)/OpenAL 1.1 SDK/include"
openal_library_directory_path = "C:/Program Files (x86)/OpenAL 1.1 SDK/libs/Win64"
nav_include_directory_path = "Nav/Header"
nav_dependency_directory_path = "Nav/Deps"
nav_api_export = "NAV_API_EXPORT"
nav_build_mode_debug = "NAV_BUILD_MODE_DEBUG"
nav_build_platform_windows = "NAV_BUILD_PLATFORM_WINDOWS"
nav_build_platform_mingw = "NAV_BUILD_PLATFORM_MINGW"
nav_build_os_windows = "NAV_BUILD_OS_WINDOWS"
nav_build_thread_win32 = "NAV_BUILD_THREAD_WIN32"
nav_build_thread_posix = "NAV_BUILD_THREAD_POSIX"
nav_build_graphic_api_d3d11 = "NAV_BUILD_GRAPHIC_API_D3D11"
nav_build_graphic_api_opengl = "NAV_BUILD_GRAPHIC_API_OPENGL"

add_rules("mode.debug", "mode.release")

-- NAV
target("nav")

    -- Lib Type
    set_kind("shared")

    -- Lib Name
    set_basename("Nav")

    -- Api Export
    add_cflags("-D" .. nav_api_export)
    add_defines(nav_api_export)

    if is_mode("debug") then

        add_cflags("-D" .. nav_build_mode_debug)
        add_defines(nav_build_mode_debug)

    end

    -- C++ Version
    set_languages(cpp_version)

    -- Include Directory
    add_includedirs(nav_include_directory_path)

    -- Include Dependencies Directory
    add_includedirs(nav_dependency_directory_path)

    -- OpenAL Include Directory
    add_includedirs(openal_include_directory_path)

    -- Platform Independent Source
    add_files("Nav/Source/Thread/*.cpp")
    add_files("Nav/Source/Memory/Arena/**.cpp")
    add_files("Nav/Source/Type/**.cpp")
    add_files("Nav/Source/Graphic/Type/**.cpp")
    add_files("Nav/Source/IO/**.cpp")

    -- Support Threads
    function build_with_posix_thread() -- Posix Thread
        
        add_cflags("-D" .. nav_build_thread_posix) -- To Header
        add_defines(nav_build_thread_posix) -- To Source
    
    end

    function build_with_win32_thread() -- Win32 Thread

        add_cflags("-D" .. nav_build_thread_win32) -- To Header
        add_defines(nav_build_thread_win32) -- To Source
    
    end

    function build_with_d3d11_graphic_api() -- Win32 Thread

        add_cflags("-D" .. nav_build_graphic_api_d3d11) -- To Header
        add_defines(nav_build_graphic_api_d3d11) -- To Source

        if is_os("windows") then

            add_files("Nav/Source/Graphic/Api/D3D11/Context/Win32/*.cpp") -- Win32 D3D11 Source
            
        end

        -- add_files("Nav/Source/Graphic/Api/D3D11/Context/*.cpp")

        add_links("d3d11")

    end

    function build_with_opengl_graphic_api() -- Win32 Thread

        add_cflags("-D" .. nav_build_graphic_api_opengl) -- To Header
        add_defines(nav_build_graphic_api_opengl) -- To Source

        if is_os("windows") then

            add_files("Nav/Source/Graphic/Api/OpenGL/Context/Win32/*.cpp") -- Win32 OpenGL Source
            
        end

        -- add_files("Nav/Source/Graphic/Api/OpenGL/Resource/*.cpp")

        add_links("OpenGL32")

    end

    -- Source Platform Windows
    if is_os("windows") then

        add_cflags("-D" .. nav_build_os_windows) -- To Header
        add_defines(nav_build_os_windows) -- To Source
    
        if is_plat("windows") then

            add_cflags("-D" .. nav_build_platform_windows) -- To Header
            add_defines(nav_build_platform_windows) -- To Source

            build_with_win32_thread() -- Support Win32 Threads

            -- Only Win32 
            add_files("Nav/Source/Net/Win32/*.cpp")
            add_links("ws2_32") -- 

        elseif is_plat("mingw") then

            add_cflags("-D" .. nav_build_platform_mingw) -- To Header
            add_defines(nav_build_platform_mingw) -- To Source

            set_prefixname("") -- No Prefix On MinGW

            build_with_win32_thread() -- Support Win32 Threads
            build_with_posix_thread() -- Support Posix Threads

        end

        -- Apis Support
        build_with_d3d11_graphic_api()
        build_with_opengl_graphic_api()
    
        add_files("Nav/Source/UI/Console/Win32/*.cpp")
        add_files("Nav/Source/UI/Window/Win32/*.cpp")
        add_files("Nav/Source/Graphic/Context/Win32/*.cpp")
        add_files("Nav/Source/Library/Win32/*.cpp")

        -- OpenAL Support
        add_files("Nav/Source/Audio/OpenAL/Win32/*.cpp")

        -- Win32 Lib
        add_links("user32")
        add_links("gdi32")

        add_links("dxgi")
        add_links("d3dcompiler")
    

        -- OpenAL Lib
        add_linkdirs(openal_library_directory_path)
        add_links("OpenAL32")

    end

target("launcher")

    set_kind("binary")

    if is_mode("debug") then

        add_defines("NAV_BUILD_MODE_DEBUG")

    end
    add_includedirs(nav_dependency_directory_path)    
    add_includedirs("Nav/Header")
    add_files("Launcher/Source/*.cpp")
    add_deps("nav")
    set_languages(cpp_version)