#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Shader.h>
#include <L/src/rendering/shader_lib.h>
#include <L/src/stream/CFileStream.h>

L_PUSH_NO_WARNINGS

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

L_POP_NO_WARNINGS

using namespace L;

static const TBuiltInResource builtin_resources = {
  /* .MaxLights = */ 32,
  /* .MaxClipPlanes = */ 6,
  /* .MaxTextureUnits = */ 32,
  /* .MaxTextureCoords = */ 32,
  /* .MaxVertexAttribs = */ 64,
  /* .MaxVertexUniformComponents = */ 4096,
  /* .MaxVaryingFloats = */ 64,
  /* .MaxVertexTextureImageUnits = */ 32,
  /* .MaxCombinedTextureImageUnits = */ 80,
  /* .MaxTextureImageUnits = */ 32,
  /* .MaxFragmentUniformComponents = */ 4096,
  /* .MaxDrawBuffers = */ 32,
  /* .MaxVertexUniformVectors = */ 128,
  /* .MaxVaryingVectors = */ 8,
  /* .MaxFragmentUniformVectors = */ 16,
  /* .MaxVertexOutputVectors = */ 16,
  /* .MaxFragmentInputVectors = */ 15,
  /* .MinProgramTexelOffset = */ -8,
  /* .MaxProgramTexelOffset = */ 7,
  /* .MaxClipDistances = */ 8,
  /* .MaxComputeWorkGroupCountX = */ 65535,
  /* .MaxComputeWorkGroupCountY = */ 65535,
  /* .MaxComputeWorkGroupCountZ = */ 65535,
  /* .MaxComputeWorkGroupSizeX = */ 1024,
  /* .MaxComputeWorkGroupSizeY = */ 1024,
  /* .MaxComputeWorkGroupSizeZ = */ 64,
  /* .MaxComputeUniformComponents = */ 1024,
  /* .MaxComputeTextureImageUnits = */ 16,
  /* .MaxComputeImageUniforms = */ 8,
  /* .MaxComputeAtomicCounters = */ 8,
  /* .MaxComputeAtomicCounterBuffers = */ 1,
  /* .MaxVaryingComponents = */ 60,
  /* .MaxVertexOutputComponents = */ 64,
  /* .MaxGeometryInputComponents = */ 64,
  /* .MaxGeometryOutputComponents = */ 128,
  /* .MaxFragmentInputComponents = */ 128,
  /* .MaxImageUnits = */ 8,
  /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
  /* .MaxCombinedShaderOutputResources = */ 8,
  /* .MaxImageSamples = */ 0,
  /* .MaxVertexImageUniforms = */ 0,
  /* .MaxTessControlImageUniforms = */ 0,
  /* .MaxTessEvaluationImageUniforms = */ 0,
  /* .MaxGeometryImageUniforms = */ 0,
  /* .MaxFragmentImageUniforms = */ 8,
  /* .MaxCombinedImageUniforms = */ 8,
  /* .MaxGeometryTextureImageUnits = */ 16,
  /* .MaxGeometryOutputVertices = */ 256,
  /* .MaxGeometryTotalOutputComponents = */ 1024,
  /* .MaxGeometryUniformComponents = */ 1024,
  /* .MaxGeometryVaryingComponents = */ 64,
  /* .MaxTessControlInputComponents = */ 128,
  /* .MaxTessControlOutputComponents = */ 128,
  /* .MaxTessControlTextureImageUnits = */ 16,
  /* .MaxTessControlUniformComponents = */ 1024,
  /* .MaxTessControlTotalOutputComponents = */ 4096,
  /* .MaxTessEvaluationInputComponents = */ 128,
  /* .MaxTessEvaluationOutputComponents = */ 128,
  /* .MaxTessEvaluationTextureImageUnits = */ 16,
  /* .MaxTessEvaluationUniformComponents = */ 1024,
  /* .MaxTessPatchComponents = */ 120,
  /* .MaxPatchVertices = */ 32,
  /* .MaxTessGenLevel = */ 64,
  /* .MaxViewports = */ 16,
  /* .MaxVertexAtomicCounters = */ 0,
  /* .MaxTessControlAtomicCounters = */ 0,
  /* .MaxTessEvaluationAtomicCounters = */ 0,
  /* .MaxGeometryAtomicCounters = */ 0,
  /* .MaxFragmentAtomicCounters = */ 8,
  /* .MaxCombinedAtomicCounters = */ 8,
  /* .MaxAtomicCounterBindings = */ 1,
  /* .MaxVertexAtomicCounterBuffers = */ 0,
  /* .MaxTessControlAtomicCounterBuffers = */ 0,
  /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
  /* .MaxGeometryAtomicCounterBuffers = */ 0,
  /* .MaxFragmentAtomicCounterBuffers = */ 1,
  /* .MaxCombinedAtomicCounterBuffers = */ 1,
  /* .MaxAtomicCounterBufferSize = */ 16384,
  /* .MaxTransformFeedbackBuffers = */ 4,
  /* .MaxTransformFeedbackInterleavedComponents = */ 64,
  /* .MaxCullDistances = */ 8,
  /* .MaxCombinedClipAndCullDistances = */ 8,
  /* .MaxSamples = */ 4,
  /* .maxMeshOutputVerticesNV = */ 256,
  /* .maxMeshOutputPrimitivesNV = */ 512,
  /* .maxMeshWorkGroupSizeX_NV = */ 32,
  /* .maxMeshWorkGroupSizeY_NV = */ 1,
  /* .maxMeshWorkGroupSizeZ_NV = */ 1,
  /* .maxTaskWorkGroupSizeX_NV = */ 32,
  /* .maxTaskWorkGroupSizeY_NV = */ 1,
  /* .maxTaskWorkGroupSizeZ_NV = */ 1,
  /* .maxMeshViewCountNV = */ 4,

  /* .limits = */ {
    /* .nonInductiveForLoops = */ 1,
    /* .whileLoops = */ 1,
    /* .doWhileLoops = */ 1,
    /* .generalUniformIndexing = */ 1,
    /* .generalAttributeMatrixVectorIndexing = */ 1,
    /* .generalVaryingIndexing = */ 1,
    /* .generalSamplerIndexing = */ 1,
    /* .generalVariableIndexing = */ 1,
    /* .generalConstantMatrixVectorIndexing = */ 1,
    },
};

static const Symbol glsl_symbol("glsl"), stage_symbol("stage"), frag_symbol("frag"), vert_symbol("vert");

#define L_GLSL_INTRO "#version 450\n#extension GL_ARB_separate_shader_objects : require\n"

bool glsl_loader(ResourceSlot& slot, Shader::Intermediate& intermediate) {
  if(slot.ext != glsl_symbol && slot.ext != frag_symbol && slot.ext != vert_symbol) {
    return false;
  }

  intermediate.stage = VK_SHADER_STAGE_ALL;
  EShLanguage shader_language;
  const Symbol stage_param = slot.parameter(stage_symbol);

  if(!strcmp(strrchr(slot.path, '.'), ".frag") || stage_param == frag_symbol) {
    intermediate.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_language = EShLanguage::EShLangFragment;
  } else if(!strcmp(strrchr(slot.path, '.'), ".vert") || stage_param == vert_symbol) {
    intermediate.stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_language = EShLanguage::EShLangVertex;
  } else {
    error("No shader language");
    return false;
  }

  static const char frag_intro[] = L_GLSL_INTRO L_SHAREDUNIFORM L_PUSH_CONSTANTS L_SHADER_LIB "\n";
  static const char vert_intro[] = L_GLSL_INTRO L_SHAREDUNIFORM L_PUSH_CONSTANTS "\n";
  const Buffer source_buffer = slot.read_source_file();

  const char* shader_strings[] = {
    intermediate.stage == VK_SHADER_STAGE_FRAGMENT_BIT ? frag_intro : vert_intro,
    (const char*)source_buffer.data(),
  };
  const int shader_string_lengths[] = {
    int(intermediate.stage == VK_SHADER_STAGE_FRAGMENT_BIT ? sizeof(frag_intro) : sizeof(vert_intro)) - 1,
    int(source_buffer.size()),
  };
  const char* shader_filenames[] = {
    "intro",
    slot.path,
  };

  const EShMessages message_flags = EShMessages(EShMessages::EShMsgSpvRules | EShMessages::EShMsgVulkanRules);

  glslang::TShader shader(shader_language);
  shader.setStringsWithLengthsAndNames(shader_strings, shader_string_lengths, shader_filenames, L_COUNT_OF(shader_strings));
  shader.setEnvInput(glslang::EShSource::EShSourceGlsl, shader_language, glslang::EShClient::EShClientVulkan, 450);
  shader.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetClientVersion::EShTargetVulkan_1_0);
  shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);
  {
    L_SCOPE_MARKER("glslang parsing");
    if(!shader.parse(&builtin_resources, 450, EProfile::ENoProfile, true, true, message_flags)) {
      warning("glslang: %s", shader.getInfoLog());
      return false;
    }
  }

  glslang::TProgram program;
  program.addShader(&shader);
  const bool link_success = program.link(message_flags);
  const bool map_success = program.mapIO();

  L_ASSERT(link_success && map_success);

  std::vector<unsigned int> spirv;
  spv::SpvBuildLogger logger;
  glslang::GlslangToSpv(*program.getIntermediate(shader_language), spirv, &logger);

  intermediate.binary = Buffer(spirv.data(), spirv.size() * sizeof(unsigned int));
  return true;
}

void glsl_module_init() {
  glslang::InitializeProcess();

  ResourceLoading<Shader>::add_loader(glsl_loader);
}
