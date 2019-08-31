#include "Pipeline.h"

#include "../engine/Engine.h"
#include "../engine/Resource.inl"
#include "Texture.h"

using namespace L;

static Symbol light_symbol("light"), present_symbol("present");

Pipeline::Pipeline(const Intermediate& intermediate) {
  { // Make sure shader bindings are not incompatible
    for(const Resource<Shader>& shader : intermediate.shaders) {
      for(const Shader::Binding& binding : shader->bindings()) {
        bool already_bound(false);
        for(Shader::Binding& own_binding : _bindings)
          if(binding.name==own_binding.name && binding.binding==own_binding.binding) {
            own_binding.stage |= binding.stage;
            already_bound = true;
            break;
          }
        if(!already_bound)
          _bindings.push(binding);
      }
    }
  }

  { // Determine render pass from name
    if(intermediate.render_pass==light_symbol) {
      _render_pass = &RenderPass::light_pass();
    } else if(intermediate.render_pass==present_symbol) {
      _render_pass = &RenderPass::present_pass();
    } else { // Default to geometry pass
      _render_pass = &RenderPass::geometry_pass();
    }
  }

  { // Create desc set layout
    Array<VkDescriptorSetLayoutBinding> dslb;
    for(const Shader::Binding& binding : _bindings) {
      if(binding.binding<0)
        continue;
      VkDescriptorType desc_type;
      switch(binding.type) {
        case Shader::BindingType::UniformConstant: desc_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; break;
        case Shader::BindingType::Uniform: desc_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
        default: continue;
      }
      dslb.push<VkDescriptorSetLayoutBinding>({uint32_t(binding.binding), desc_type, 1, binding.stage});
    }

    {
      VkDescriptorSetLayoutCreateInfo create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      create_info.bindingCount = uint32_t(dslb.size());
      create_info.pBindings = dslb.begin();

      L_VK_CHECKED(vkCreateDescriptorSetLayout(Vulkan::device(), &create_info, nullptr, &_desc_set_layout));
    }

    { // Create pipeline layout
      VkDescriptorSetLayout layouts[] {_desc_set_layout};
      VkPipelineLayoutCreateInfo create_info = {};
      create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      create_info.setLayoutCount = L_COUNT_OF(layouts);
      create_info.pSetLayouts = layouts;
      VkPushConstantRange push_constant_range;
      if(const Shader::Binding* constants_binding = find_binding("Constants")) {
        push_constant_range = {constants_binding->stage, 0, 64};
        create_info.pushConstantRangeCount = 1;
        create_info.pPushConstantRanges = &push_constant_range;
      }

      L_VK_CHECKED(vkCreatePipelineLayout(Vulkan::device(), &create_info, nullptr, &_layout));
    }
  }


  VkPipelineShaderStageCreateInfo shader_stages[4] {};
  uint32_t stage_count(0);
  for(Resource<Shader> shader : intermediate.shaders) {
    VkPipelineShaderStageCreateInfo& shader_stage(shader_stages[stage_count++]);
    shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage.stage = shader->stage();
    shader_stage.module = shader->module();
    shader_stage.pName = "main";
  }

  uint32_t vertex_size(0);
  Array<VkVertexInputAttributeDescription> vertex_attributes;
  for(const Shader::Binding& binding : _bindings) {
    if(binding.stage==VK_SHADER_STAGE_VERTEX_BIT && binding.type==Shader::BindingType::Input) {
      VkVertexInputAttributeDescription vertex_attribute {};
      vertex_size = max<uint32_t>(vertex_size, binding.offset+binding.size);
      vertex_attribute.binding = binding.binding;
      vertex_attribute.location = binding.index;
      vertex_attribute.format = binding.format;
      vertex_attribute.offset = binding.offset;
      vertex_attributes.push(vertex_attribute);
    }
  }

  VkVertexInputBindingDescription vertex_binding {};
  vertex_binding.binding = 0;
  vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  vertex_binding.stride = vertex_size;

  VkPipelineVertexInputStateCreateInfo vertex_input {};
  vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  if(vertex_size>0) {
    vertex_input.vertexBindingDescriptionCount = 1;
    vertex_input.pVertexBindingDescriptions = &vertex_binding;

    vertex_input.vertexAttributeDescriptionCount = uint32_t(vertex_attributes.size());
    vertex_input.pVertexAttributeDescriptions = vertex_attributes.begin();
  }

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  static const VkRect2D scissor = {{0, 0}, {1<<16, 1<<16}};
  static const VkViewport viewport = {};

  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = intermediate.cull_mode;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f; // Optional
  multisampling.pSampleMask = nullptr; // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE; // Optional

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  if(intermediate.blend_override!=BlendMode::None) {
    switch(intermediate.blend_override) {
      case BlendMode::Mult:
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        break;
      default: break;
    }
  } else if(_render_pass==&RenderPass::light_pass()) {
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  } else if(_render_pass==&RenderPass::present_pass()) {
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  }

  VkPipelineColorBlendAttachmentState color_blend_attachments[] {
    colorBlendAttachment,
    colorBlendAttachment,
  };

  VkPipelineDepthStencilStateCreateInfo depth_stencil {};
  depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = VK_TRUE;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
  color_blending.attachmentCount = _render_pass->color_attachment_count();
  color_blending.pAttachments = color_blend_attachments;

  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT
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
  create_info.pInputAssemblyState = &inputAssembly;
  create_info.pViewportState = &viewportState;
  create_info.pRasterizationState = &rasterizer;
  create_info.pMultisampleState = &multisampling;
  create_info.pDepthStencilState = &depth_stencil; // Optional
  create_info.pColorBlendState = &color_blending;
  create_info.pDynamicState = &dynamic_state_create_info;
  create_info.layout = _layout;
  create_info.renderPass = *_render_pass;
  create_info.subpass = 0;
  create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
  create_info.basePipelineIndex = -1; // Optional

  L_VK_CHECKED(vkCreateGraphicsPipelines(Vulkan::device(), VK_NULL_HANDLE, 1, &create_info, nullptr, &_pipeline));
}
Pipeline::~Pipeline() {
  vkDestroyPipeline(Vulkan::device(), _pipeline, nullptr);
  vkDestroyPipelineLayout(Vulkan::device(), _layout, nullptr);
}

const Shader::Binding* Pipeline::find_binding(const Symbol& name) const {
  for(const Shader::Binding& binding : _bindings)
    if(binding.name==name)
      return &binding;
  return nullptr;
}
