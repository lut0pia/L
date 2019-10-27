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
  warnings "Extra"
  flags {"FatalWarnings"}

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
    defines {"_CRT_SECURE_NO_WARNINGS", "_WINSOCK_DEPRECATED_NO_WARNINGS"}
    characterset "MBCS" -- Don't use UNICODE
    links {"ws2_32","vulkan-1"}
  filter {"action:vs*", "Debug or Development"}
    links {"dbghelp"}
  filter {"action:vs*", "Release"}
    linkoptions {"/NODEFAULTLIB:libcmtd.lib"}

  -- GMake
  configuration {"gmake2"}
    buildoptions {"-fno-operator-names"}
    disablewarnings {"missing-field-initializers", "type-limits", "missing-braces"}
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
      filter {}
      removefiles {"src/module/**"} -- Start by exluding all modules
      local modules = os.matchdirs("src/module/*")
      local module_init_str = ""
      for _,modulepath in pairs(modules) do
        local module = path.getname(modulepath)
        local moduleswitch = "L_USE_MODULE_" .. module
        local moduleconf = loadfile(modulepath .. "/premake5.lua")
        if moduleconf then
          moduleconf()
        else
          filter {}
        end
        defines {moduleswitch}
        files {modulepath .. "/**.cpp"}
        module_init_str = module_init_str .. ("#if " .. moduleswitch .. "\n"
        .. "{\n"
        .. "L_SCOPE_MARKER(\"" .. module .. "\");\n"
        .. "extern void " .. module .. "_module_init();" .. module .. "_module_init();\n"
        .. "}\n"
        .. "#endif\n")
      end
      local module_init_file_name = "src/module/init.gen"
      local previous_module_init_str = io.readfile(module_init_file_name)
      if module_init_str ~= previous_module_init_str then
        io.writefile(module_init_file_name, module_init_str)
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
