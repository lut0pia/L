#!lua
solution "L"
  configurations {"Debug", "Development", "Release"}

  -- General options
  location("prj/".._ACTION)
  staticruntime "On"
  cppdialect "C++11"
  floatingpoint "Fast"
  exceptionhandling "Off"
  rtti "Off"
  editandcontinue "Off"

  -- Platform
  configuration {"windows"}
    architecture "x64"
    defines {"L_WINDOWS","WIN32_LEAN_AND_MEAN","NOMINMAX","NOCRYPT"}
    local vk_sdk = os.getenv("VULKAN_SDK")
    if vk_sdk then
      includedirs {vk_sdk.."/Include"}
      libdirs {vk_sdk.."/Lib"}
    end
  configuration {"linux"}
    defines {"L_UNIX"}

  -- Visual Studio
  filter {"action:vs*"}
    files {"src/**.natvis","src/**.rc"}
    linkoptions {"/NODEFAULTLIB:libc.lib","/NODEFAULTLIB:msvcrt.lib","/NODEFAULTLIB:libcd.lib","/NODEFAULTLIB:msvcrtd.lib"}
    defines {"_CRT_SECURE_NO_WARNINGS","_WINSOCK_DEPRECATED_NO_WARNINGS"}
    buildoptions {"/wd4100"} -- Unreferenced formal parameter
    buildoptions {"/wd4146"} -- Unary minus operator applied to unsigned type
    characterset "MBCS" -- Don't use UNICODE
    warnings "Extra"
    links {"ws2_32","vulkan-1"}
  filter {"action:vs*", "Debug or Development"}
    links {"dbghelp"}
  filter {"action:vs*", "Release"}
    linkoptions {"/NODEFAULTLIB:libcmtd.lib"}

  -- GMake
  configuration {"gmake2"}
    buildoptions {"-fno-operator-names"}
    links {"vulkan","X11","pthread"}

  -- Main project (startup)
  project "Engine"
    language "C++"
    files {"src/**.h","src/**.cpp","src/**.inl"}
    debugdir "smp"
    targetdir "smp"
    includedirs {".."}

    -- Exclude system-specific files
    configuration {"not windows"}
      excludes {"**_win**"}
    configuration {"not linux"}
      excludes {"**_unix**"}

    filter {"configurations:Release"}
      excludes {"src/pipeline/**"}

    do -- Modules
      configuration {}
      removefiles {"src/module/**"} -- Start by exluding all modules
      local modules = os.matchdirs("src/module/*")
      local moduleinit = io.open("src/module/init.gen","w")
      for _,modulepath in pairs(modules) do
        local module = path.getname(modulepath)
        local moduleswitch = "L_USE_MODULE_" .. module
        local moduleconf = loadfile(modulepath .. "/premake5.lua")
        if moduleconf then
          moduleconf()
        else
          configuration {}
        end
        defines {moduleswitch}
        files {modulepath .. "/**.cpp"}
        moduleinit:write("#if " .. moduleswitch .. "\n"
        .. "{\n"
        .. "L_SCOPE_MARKER(\"" .. module .. "\");\n"
        .. "extern void " .. module .. "_module_init();" .. module .. "_module_init();\n"
        .. "}\n"
        .. "#endif\n")
      end
    end
    
    -- PCH
    configuration {}
    if _ACTION ~= "gmake2" then
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
