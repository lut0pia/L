#!lua
solution "L"
	configurations {"Debug", "Release"}
	
	-- General options
	location("prj/".._ACTION)
	defines {"GLEW_STATIC"}
	flags {"StaticRuntime"}
	
	-- Platform
	configuration {"windows"}
		defines {"L_WINDOWS","WIN32_LEAN_AND_MEAN","NOMINMAX","NOCRYPT"}
	configuration {"linux"}
		defines {"L_UNIX"}
	
	-- Visual Studio
	configuration {"vs*"}
		includedirs {"ext/include"}
		libdirs {"ext/lib"}
		linkoptions {"/NODEFAULTLIB:libc.lib","/NODEFAULTLIB:msvcrt.lib","/NODEFAULTLIB:libcd.lib","/NODEFAULTLIB:msvcrtd.lib"}
		defines {"_CRT_SECURE_NO_WARNINGS"}
		buildoptions {"/MP","/wd4200","/wd4146","/wd4244"}
		characterset "MBCS" -- Don't use UNICODE
	configuration {"vs*","Debug"}
		linkoptions {"/NODEFAULTLIB:libcmt.lib"}
	configuration {"vs*","Release"}
		linkoptions {"/NODEFAULTLIB:libcmtd.lib"}
		
	-- Test project (startup)
	project "Test"
		targetdir "bin"
		targetname "Test"
		kind "ConsoleApp"
		language "C++"
		files {"test/**.h","test/**.cpp"}
		debugdir("test")
		includedirs {".."}
		libdirs {"bin"}
		links {"L"}
		
		configuration {"vs*","Debug"}
			objdir("obj/".._ACTION.."/test/debug")
			defines {"L_DEBUG"}
			flags {"Symbols"}
			
		configuration  {"vs*","Release"}
			objdir("obj/".._ACTION.."/test/release")
			flags {"Optimize"}
	
	-- Library project
	project "L"
		targetdir "bin"
		kind "StaticLib"
		language "C++"
		files {"src/**.h","src/**.cpp"}
		excludes {"src/interface/**"} -- Interface files are not to be compiled by the library
		
		-- PCH
		pchheader "pc.h"
		pchsource "src/pc.cpp"
		forceincludes {"pc.h"}
		
		-- Visual Studio
		configuration {"vs*"}
			postbuildcommands {[[lib.exe /LTCG /NOLOGO /IGNORE:4006,4221 /LIBPATH:../../ext/lib /OUT:"$(TargetPath)" "$(TargetPath)" gdi32.lib user32.lib opengl32.lib glew32s.lib glu32.lib ws2_32.lib]]}
		
		-- Visual Studio Debug
		configuration {"vs*","Debug"}
			objdir("obj/".._ACTION.."/debug")
			defines {"L_DEBUG"}
			flags {"Symbols"}
		
		-- Visual Studio Release
		configuration  {"vs*","Release"}
			objdir("obj/".._ACTION.."/release")
			flags {"Optimize"}
	
	