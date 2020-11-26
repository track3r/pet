workspace "Pet"
	location "project"
	language "C++"
    cppdialect "C++14"
    architecture "x86_64"    

    configurations { "debug", "release" }
	filter { "configurations:debug" }
		symbols "On"
	filter { "configurations:release" }
        optimize "On"
        symbols "On"
	
	filter { }
	
	targetdir ("build/bin/%{prj.name}/%{cfg.longname}")
    objdir ("build/obj/%{prj.name}/%{cfg.longname}")
    
project "cave"
	kind "WindowedApp"
    files { "cave/**.cpp", "cave/**.h" }
    removefiles "cave/ECS/**"
    removefiles "cave/ECS.cpp"

    pchheader "pch.h"
    pchsource "cave/pch.cpp"

	includedirs { "libs/glm", "libs/glew", "libs/sdl", "libs/stb", "cave" }
    libdirs { "libs/glew" }
    
    filter { "system:windows" }
        links { "opengl32", "SDL2", "SDL2main" }

    filter { "configurations:debug" }
        libdirs { "libs/sdl/win64_dbg" }
        defines { "OPENGL", "WIN32", "DEBUG", "_WINDOWS", "GLEW_STATIC", "_CRT_SECURE_NO_WARNINGS" }
        filter { "system:windows" }
            links { "glew32sd"}

    filter { "configurations:release" }
        libdirs { "libs/sdl/win64_dbg" }
        defines { "OPENGL", "WIN32", "NDEBUG", "_WINDOWS", "GLEW_STATIC", "_CRT_SECURE_NO_WARNINGS" }
            filter { "system:windows" }
                links { "glew32s"}	
                
    filter { "system:windows" }
        postbuildcommands {
            "{COPY} ../libs/sdl/win64_dbg/SDL2.dll %{cfg.targetdir}"
        }
    --files {"libs/sdl/win64_dbg/SDL2.dll"}
    --filter "files:**.dll"
        --buildaction "Copy"
	--filter { "system:not windows" }
	    --links { "GL" }