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
	configuration {"linux"}
		defines {"L_UNIX"}

	-- Visual Studio
	configuration {"vs*"}
		libdirs {"ext/lib"}
		linkoptions {"/NODEFAULTLIB:libc.lib","/NODEFAULTLIB:msvcrt.lib","/NODEFAULTLIB:libcd.lib","/NODEFAULTLIB:msvcrtd.lib"}
		defines {"_CRT_SECURE_NO_WARNINGS","_WINSOCK_DEPRECATED_NO_WARNINGS"}
		buildoptions {"/wd4100","/wd4146","/wd4200","/wd4244","/wd4702","/wd4706","/wd4577","/wd4592"}
		characterset "MBCS" -- Don't use UNICODE
		warnings "Extra"
	configuration {"vs*","Debug"}
		linkoptions {"/NODEFAULTLIB:libcmt.lib"}
	configuration {"vs*","Release"}
		linkoptions {"/NODEFAULTLIB:libcmtd.lib"}

	-- GMake
	configuration {"gmake"}
		buildoptions {"-fno-operator-names"}
    links {"GL","GLU","X11"}

	-- Sample project (startup)
	project "Sample"
		language "C++"
		files {"src/main.cpp"}
		debugdir "smp"
		targetdir "smp"
		includedirs {".."}
		libdirs {"bin"}
		links {"L"}

		configuration {"Debug"}
			targetname "Ldbg"
			kind "ConsoleApp"
			objdir("obj/".._ACTION.."/smp/dbg")
			defines {"L_DEBUG"}
			symbols "On"

		configuration {"Development"}
			targetname "Ldev"
			kind "ConsoleApp"
			objdir("obj/".._ACTION.."/smp/dev")
			defines {"L_DEBUG"}
			symbols "On"
			optimize "On"

		configuration {"Release"}
			targetname "L"
			kind "WindowedApp"
			objdir("obj/".._ACTION.."/smp/rls")
			optimize "On"

	-- Library project
	project "L"
		targetdir "bin"
		kind "StaticLib"
		language "C++"
		files {"src/**.h","src/**.cpp"}
		excludes {"src/main.cpp","src/interface/**"} -- Interface files are not to be compiled by the library

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

		-- Visual Studio
		filter {"action:vs*"}
			files {"src/**.natvis"}
		local staticlinkcmd = [[lib.exe /LTCG /NOLOGO /IGNORE:4006,4221 /OUT:"$(TargetPath)" "$(TargetPath)" ]]
		local staticlinks = "user32.lib opengl32.lib ws2_32.lib hid.lib winmm.lib"
		filter {"action:vs*", "Debug or Development"}
			postbuildcommands {staticlinkcmd..staticlinks.." dbghelp.lib"}
		filter {"action:vs*", "Release"}
			postbuildcommands {staticlinkcmd..staticlinks}

		configuration {"Debug"}
			targetdir "bin/dbg"
			objdir("obj/".._ACTION.."/dbg")
			defines {"L_DEBUG"}
			symbols "On"

		configuration {"Development"}
			targetdir "bin/dev"
			objdir("obj/".._ACTION.."/dev")
			defines {"L_DEBUG"}
			symbols "On"
			optimize "On"

		configuration  {"Release"}
			targetdir "bin/rls"
			objdir("obj/".._ACTION.."/rls")
			optimize "On"
