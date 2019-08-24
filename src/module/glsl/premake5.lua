filter {"configurations:not Release"}
  includedirs {os.getcwd() .. "/include"}
  libdirs {os.getcwd() .. "/lib"}
filter {"configurations:Debug", "system:Windows"}
  links {"glslangd", "HLSLd", "OGLCompilerd", "OSDependentd", "SPIRVd"}
filter {"configurations:Development", "system:Windows"}
  links {"glslang", "HLSL", "OGLCompiler", "OSDependent", "SPIRV"}
filter {"configurations:not Release", "system:Linux"}
  links {"glslang", "HLSL", "OGLCompiler", "OSDependent", "SPIRV"}
filter {"configurations:not Release"} -- Last filter for module activation
