filter {"configurations:not Release"}
  includedirs {os.getcwd() .. "/include"}
  libdirs {os.getcwd() .. "/lib"}
filter {"configurations:Debug", "system:not Linux"}
  links {"glslangd", "HLSLd", "OGLCompilerd", "OSDependentd", "SPIRVd"}
filter {"configurations:Development or system:Linux"}
  links {"glslang", "HLSL", "OGLCompiler", "OSDependent", "SPIRV"}
filter {"configurations:not Release"} -- Last filter for module activation
