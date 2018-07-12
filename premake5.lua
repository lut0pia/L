#!lua
solution "L"
	configurations {"Debug", "Development", "Release"}

	-- General options
	location("prj/".._ACTION)
  includedirs {"ext/include"}
	flags {"StaticRuntime"}
	cppdialect "C++11"
	floatingpoint "Fast"
	exceptionhandling "Off"
	rtti "Off"
	editandcontinue "Off"

	-- Platform
	configuration {"windows"}
		architecture "x64"
		defines {"L_WINDOWS","WIN32_LEAN_AND_MEAN","NOMINMAX","NOCRYPT"}
		includedirs {"C:/VulkanSDK/1.1.73.0/Include"}
		libdirs {"C:/VulkanSDK/1.1.73.0/Lib"}
	configuration {"linux"}
		defines {"L_UNIX"}

	-- Visual Studio
	filter {"action:vs*"}
		files {"src/**.natvis"}
		linkoptions {"/NODEFAULTLIB:libc.lib","/NODEFAULTLIB:msvcrt.lib","/NODEFAULTLIB:libcd.lib","/NODEFAULTLIB:msvcrtd.lib"}
		defines {"_CRT_SECURE_NO_WARNINGS","_WINSOCK_DEPRECATED_NO_WARNINGS"}
		buildoptions {"/wd4100","/wd4146","/wd4200","/wd4244","/wd4702","/wd4706","/wd4577","/wd4592"}
		characterset "MBCS" -- Don't use UNICODE
		warnings "Extra"
		links {"user32","opengl32","ws2_32","hid","winmm","vulkan-1"}
	filter {"action:vs*", "Debug or Development"}
		links {"dbghelp"}
	filter {"action:vs*", "Release"}
		linkoptions {"/NODEFAULTLIB:libcmtd.lib"}

	-- GMake
	configuration {"gmake"}
		buildoptions {"-fno-operator-names"}
    links {"GL","vulkan","X11","pthread","asound"}

	-- Main project (startup)
	project "Engine"
		language "C++"
		files {"src/**.h","src/**.cpp"}
		excludes {"src/interface/**"} -- Interface files are not to be compiled by the library
		debugdir "smp"
		targetdir "smp"
		includedirs {".."}

		-- Exclude system-specific files
		configuration {"not windows"}
			excludes {"**_win**"}
		configuration {"not linux"}
			excludes {"**_unix**"}
		configuration {}
		
		-- PCH
    if _ACTION ~= "gmake" then
  		pchheader "pc.h"
  		pchsource "src/pc.cpp"
			forceincludes {"pc.h"}
		else
      pchheader "src/pc.h"
    end

		configuration {"Debug"}
			targetname "Ldbg"
			kind "ConsoleApp"
			objdir("obj/".._ACTION.."/dbg")
			defines {"L_DEBUG"}
			symbols "On"

		configuration {"Development"}
			targetname "Ldev"
			kind "ConsoleApp"
			objdir("obj/".._ACTION.."/dev")
			defines {"L_DEBUG"}
			symbols "On"
			optimize "On"

		configuration {"Release"}
			targetname "L"
			kind "WindowedApp"
			objdir("obj/".._ACTION.."/rls")
			optimize "On"
