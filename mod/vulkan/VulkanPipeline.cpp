#include "VulkanRenderer.h"

#include <L/src/rendering/RenderPass.h>
#include <L/src/rendering/Shader.h>

using namespace L;

const ShaderBinding* VulkanPipeline::find_binding(const Symbol& name) const {
  for(const ShaderBinding& binding : bindings)
    if(binding.name == name)
      return &binding;
  return nullptr;
}

PipelineImpl* VulkanRenderer::create_pipeline(
  const L::ShaderImpl* const* shaders,
  size_t shader_count,
  const ShaderBinding* bindings,
  size_t binding_count,
  const L::VertexAttribute* in_vertex_attributes,
  size_t in_vertex_attribute_count,
  const L::RenderPass& render_pass,
  L::PolygonMode polygon_mode,
  L::CullMode cull_mode,
  L::PrimitiveTopology topology,
  L::BlendMode blend_mode) {
  VulkanPipeline* pipeline = Memory::new_type<VulkanPipeline>();

  { // Create desc set layout
    Array<VkDescriptorSetLayoutBinding> dslb;
    for(uintptr_t i = 0; i < binding_count; i++) {
      const ShaderBinding& binding = bindings[i];
      pipeline->bindings.push(binding);
      if(binding.binding < 0) {
        continue;
      }
      VkDescriptorType desc_type;
      switch(binding.type) {
        case ShaderBindingType::UniformConstant: desc_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; break;
        case ShaderBindingType::Uniform: desc_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
        default: continue;
      }
      dslb.push<VkDescriptorSetLayoutBinding>({uint32_t(binding.binding), desc_type, 1, VK_SHADER_STAGE_ALL});
    }

    {
      VkDescriptorSetLayoutCreateInfo create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      create_info.bindingCount = uint32_t(dslb.size());
      create_info.pBindings = dslb.begin();

      L_VK_CHECKED(vkCreateDescriptorSetLayout(_device, &create_info, nullptr, &pipeline->desc_set_layout));
    }

    { // Create pipeline layout
      VkDescriptorSetLayout layouts[] {pipeline->desc_set_layout};
      VkPipelineLayoutCreateInfo create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      create_info.setLayoutCount = L_COUNT_OF(layouts);
      create_info.pSetLayouts = layouts;
      VkPushConstantRange push_constant_range;
      if(pipeline->find_binding("Constants") != nullptr) {
        push_constant_range = {VK_SHADER_STAGE_ALL, 0, 64};
        create_info.pushConstantRangeCount = 1;
        create_info.pPushConstantRanges = &push_constant_range;
      }

      L_VK_CHECKED(vkCreatePipelineLayout(_device, &create_info, nullptr, &pipeline->layout));
    }
  }

  VkPipelineShaderStageCreateInfo shader_stages[4] {};
  uint32_t stage_count(0);
  for(uintptr_t i = 0; i < shader_count; i++) {
    VkPipelineShaderStageCreateInfo& shader_stage(shader_stages[stage_count++]);
    shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage.stage = VkShaderStageFlagBits(((VulkanShader*)shaders[i])->stage);
    shader_stage.module = ((VulkanShader*)shaders[i])->module;
    shader_stage.pName = "main";
  }

  size_t vertex_size = 0;
  Array<VkVertexInputAttributeDescription> vertex_attributes;
  if(in_vertex_attribute_count == 0) {
    for(const ShaderBinding& binding : pipeline->bindings) {
      if(binding.stage == ShaderStage::Vertex && binding.type == ShaderBindingType::Input) {
        VkVertexInputAttributeDescription vertex_attribute {};
        vertex_size = max<size_t>(vertex_size, binding.offset + binding.size);
        vertex_attribute.binding = binding.binding;
        vertex_attribute.location = binding.index;
        vertex_attribute.format = to_vk_format(binding.format);
        vertex_attribute.offset = binding.offset;
        vertex_attributes.push(vertex_attribute);
      }
    }
  } else {
    uint32_t location = 0;
    for(uintptr_t i = 0; i < in_vertex_attribute_count; i++) {
      const VertexAttribute& attribute = in_vertex_attributes[i];
      VkVertexInputAttributeDescription vertex_attribute {};
      vertex_attribute.location = location++;
      vertex_attribute.format = to_vk_format(attribute.format);
      vertex_attribute.offset = uint32_t(vertex_size);
      vertex_size += Renderer::format_size(attribute.format);
      vertex_attributes.push(vertex_attribute);
    }
  }

  VkVertexInputBindingDescription vertex_binding {};
  vertex_binding.binding = 0;
  vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  vertex_binding.stride = uint32_t(vertex_size);

  VkPipelineVertexInputStateCreateInfo vertex_input {};
  vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  if(vertex_size > 0) {
    vertex_input.vertexBindingDescriptionCount = 1;
    vertex_input.pVertexBindingDescriptions = &vertex_binding;

    vertex_input.vertexAttributeDescriptionCount = uint32_t(vertex_attributes.size());
    vertex_input.pVertexAttributeDescriptions = vertex_attributes.begin();
  }

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = to_vk_topology(topology);
  input_assembly.primitiveRestartEnable = VK_FALSE;

  static const VkRect2D scissor = {{0, 0}, {1 << 16, 1 << 16}};
  static const VkViewport viewport = {};

  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = to_vk_polygon_mode(polygon_mode);
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = to_vk_cull_mode(cull_mode);
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f; // Optional
  multisampling.pSampleMask = nullptr; // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE; // Optional

  VkPipelineDepthStencilStateCreateInfo depth_stencil {};
  depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = VK_TRUE;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;

  VkPipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  switch(blend_mode) {
    case BlendMode::Mult:
      color_blend_attachment.blendEnable = VK_TRUE;
      color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
      color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
      color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
      break;
    case BlendMode::Add:
      color_blend_attachment.blendEnable = VK_TRUE;
      color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
      color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
      color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
      break;
    case BlendMode::Alpha:
      color_blend_attachment.blendEnable = VK_TRUE;
      color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
      color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
      break;
    case BlendMode::None: break;
    default: error("vulkan: Unsupported blend mode"); break;
  }

  VkPipelineColorBlendAttachmentState color_blend_attachments[] {
    color_blend_attachment,
    color_blend_attachment,
  };

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
  color_blending.attachmentCount = render_pass.color_attachment_count();
  color_blending.pAttachments = color_blend_attachments;

  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
  };

  VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
  dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_create_info.pDynamicStates = dynamic_states;
  dynamic_state_create_info.dynamicStateCount = L_COUNT_OF(dynamic_states);

  VkGraphicsPipelineCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  create_info.stageCount = stage_count;
  create_info.pStages = shader_stages;
  create_info.pVertexInputState = &vertex_input;
  create_info.pInputAssemblyState = &input_assembly;
  create_info.pViewportState = &viewport_state;
  create_info.pRasterizationState = &rasterizer;
  create_info.pMultisampleState = &multisampling;
  create_info.pDepthStencilState = &depth_stencil; // Optional
  create_info.pColorBlendState = &color_blending;
  create_info.pDynamicState = &dynamic_state_create_info;
  create_info.layout = pipeline->layout;
  create_info.renderPass = (VkRenderPass)render_pass.get_impl();
  create_info.subpass = 0;
  create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
  create_info.basePipelineIndex = -1; // Optional

  L_VK_CHECKED(vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline->pipeline));

  return pipeline;
}
void VulkanRenderer::destroy_pipeline(PipelineImpl* pipeline) {
  VulkanPipeline* vk_pipeline = (VulkanPipeline*)pipeline;
  vkDestroyPipeline(_device, vk_pipeline->pipeline, nullptr);
  vkDestroyPipelineLayout(_device, vk_pipeline->layout, nullptr);

  // Destroy all related descriptor sets
  for(uintptr_t i = 0; i < free_sets.size(); i++) {
    const FlyingSet& flying_set = free_sets[i];
    if(flying_set.pipeline == vk_pipeline->pipeline) {
      vkFreeDescriptorSets(_device, _descriptor_pool, 1, &flying_set.set);
      free_sets.erase_fast(i);
    }
  }
  for(uintptr_t i = 0; i < used_sets.size(); i++) {
    const FlyingSet& flying_set = used_sets[i];
    if(flying_set.pipeline == vk_pipeline->pipeline) {
      vkFreeDescriptorSets(_device, _descriptor_pool, 1, &flying_set.set);
      used_sets.erase_fast(i);
    }
  }

  Memory::delete_type(vk_pipeline);
}
