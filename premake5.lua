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
    local vulkansdks = os.matchdirs("C:/VulkanSDK/*")
    if #vulkansdks >0 then
      includedirs {vulkansdks[1].."/Include"}
      libdirs {vulkansdks[1].."/Lib"}
    end
  configuration {"linux"}
    defines {"L_UNIX"}
    includedirs {"/home/lutopia/Downloads/VulkanSDK/1.1.73.0/x86_64/include"}
    libdirs {"/home/lutopia/Downloads/VulkanSDK/1.1.73.0/x86_64/lib"}

  -- Visual Studio
  filter {"action:vs*"}
    files {"src/**.natvis","src/**.rc"}
    linkoptions {"/NODEFAULTLIB:libc.lib","/NODEFAULTLIB:msvcrt.lib","/NODEFAULTLIB:libcd.lib","/NODEFAULTLIB:msvcrtd.lib"}
    defines {"_CRT_SECURE_NO_WARNINGS","_WINSOCK_DEPRECATED_NO_WARNINGS"}
    buildoptions {"/wd4100","/wd4146","/wd4200","/wd4244","/wd4702","/wd4706","/wd4577","/wd4592"}
    characterset "MBCS" -- Don't use UNICODE
    warnings "Extra"
    links {"ws2_32","vulkan-1"}
  filter {"action:vs*", "Debug or Development"}
    links {"dbghelp"}
  filter {"action:vs*", "Release"}
    linkoptions {"/NODEFAULTLIB:libcmtd.lib"}

  -- GMake
  configuration {"gmake"}
    buildoptions {"-fno-operator-names"}
    links {"vulkan","X11","pthread"}

  -- Main project (startup)
  project "Engine"
    language "C++"
    files {"src/**.h","src/**.cpp"}
    debugdir "smp"
    targetdir "smp"
    includedirs {".."}

    -- Exclude system-specific files
    configuration {"not windows"}
      excludes {"**_win**"}
    configuration {"not linux"}
      excludes {"**_unix**"}
    configuration {}

    do -- Modules
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
        .. "extern void " .. module .. "_module_init();" .. module .. "_module_init();\n"
        .. "#endif\n")
      end
      configuration {}
    end
    
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
