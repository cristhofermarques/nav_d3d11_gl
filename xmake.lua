cppVersion = "c++17"

add_rules("mode.debug", "mode.release")

-- NAV
target("nav")

    -- Lib Type
    set_kind("shared")

    -- Lib Name
    set_basename("Nav")

    -- Api Export
    add_cflags("-DNAV_API_EXPORT")

    if is_mode("debug") then

        -- add_cflags("-DNAV_BUILD_MODE_DEBUG")
        add_defines("NAV_BUILD_MODE_DEBUG")

    end

    -- C++ Version
    set_languages(cppVersion)

    -- Include Directory
    add_includedirs("Nav/Header")

    -- Include Dependencies Directory
    add_includedirs("Nav/Deps")

    -- Source Platform Independent
    add_files("Nav/Source/Thread/*.cpp")

    -- Source Platform Windows
    if is_os("windows") then

        add_defines("NAV_BUILD_PLATFORM_WINDOWS")
    
        if is_plat("windows") then

            add_defines("NAV_BUILD_THREAD_WIN32")

            add_files("Nav/Source/Thread/Win32/*.cpp")
            add_files("Nav/Source/Net/Win32/*.cpp")

            add_links("ws2_32")

        elseif is_plat("mingw") then

            set_prefixname("")

            add_defines("NAV_BUILD_THREAD_WIN32")
            add_defines("NAV_BUILD_THREAD_POSIX")

            add_files("Nav/Source/Thread/Posix/*.cpp")
            add_files("Nav/Source/Thread/Win32/*.cpp")

        end

        -- D3D Support
        add_files("Nav/Source/Graphic/D3D11/Win32/*.cpp")
        
        add_files("Nav/Source/UI/Console/Win32/*.cpp")
        add_files("Nav/Source/UI/Window/Win32/*.cpp")
        add_files("Nav/Source/Graphic/Win32/*.cpp")

        add_links("user32")
        add_links("gdi32")
        add_links("dxgi")
        add_links("d3d11")

    end

target("launcher")
    set_kind("binary")

    if is_mode("debug") then

        -- add_cflags("-DNAV_BUILD_MODE_DEBUG")
        add_defines("NAV_BUILD_MODE_DEBUG")

    end
    
    add_includedirs("Nav/Header")
    add_files("Launcher/Source/*.cpp")
    add_deps("nav")
    set_languages(cppVersion)
