#include "Pipeline.h"

#include "../engine/Engine.h"
#include "../engine/Resource.inl"
#include "Texture.h"

using namespace L;

static Symbol light_symbol("light"), present_symbol("present");

Pipeline::Pipeline(const Parameters& parameters) {
  { // Make sure shader bindings are not incompatible
    for(const auto& shader : parameters.shaders) {
      for(const Shader::Binding& binding : shader.value()->bindings()) {
        bool already_bound(false);
        for(Shader::Binding& own_binding : _bindings)
          if(binding.name == own_binding.name && binding.binding == own_binding.binding) {
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
    if(parameters.render_pass == light_symbol) {
      _render_pass = &RenderPass::light_pass();
    } else if(parameters.render_pass == present_symbol) {
      _render_pass = &RenderPass::present_pass();
    } else { // Default to geometry pass
      _render_pass = &RenderPass::geometry_pass();
    }
  }

  { // Create desc set layout
    Array<VkDescriptorSetLayoutBinding> dslb;
    for(const Shader::Binding& binding : _bindings) {
      if(binding.binding < 0)
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
  for(auto& shader : parameters.shaders) {
    VkPipelineShaderStageCreateInfo& shader_stage(shader_stages[stage_count++]);
    shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage.stage = shader.value()->stage();
    shader_stage.module = shader.value()->module();
    shader_stage.pName = "main";
  }

  uint32_t vertex_size(0);
  Array<VkVertexInputAttributeDescription> vertex_attributes;
  if(parameters.vertex_attributes.empty()) {
    for(const Shader::Binding& binding : _bindings) {
      if(binding.stage == VK_SHADER_STAGE_VERTEX_BIT && binding.type == Shader::BindingType::Input) {
        VkVertexInputAttributeDescription vertex_attribute {};
        vertex_size = max<uint32_t>(vertex_size, binding.offset + binding.size);
        vertex_attribute.binding = binding.binding;
        vertex_attribute.location = binding.index;
        vertex_attribute.format = binding.format;
        vertex_attribute.offset = binding.offset;
        vertex_attributes.push(vertex_attribute);
      }
    }
  } else {
    uint32_t location = 0;
    for(const VertexAttribute& attribute : parameters.vertex_attributes) {
      VkVertexInputAttributeDescription vertex_attribute {};
      vertex_attribute.location = location++;
      vertex_attribute.format = attribute.format;
      vertex_attribute.offset = vertex_size;
      vertex_size += Vulkan::format_size(attribute.format);
      vertex_attributes.push(vertex_attribute);
    }
  }

  VkVertexInputBindingDescription vertex_binding {};
  vertex_binding.binding = 0;
  vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  vertex_binding.stride = vertex_size;

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
  input_assembly.topology = parameters.topology == VK_PRIMITIVE_TOPOLOGY_MAX_ENUM ? VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : parameters.topology;
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
  rasterizer.polygonMode = parameters.polygon_mode == VK_POLYGON_MODE_MAX_ENUM ? VK_POLYGON_MODE_FILL : parameters.polygon_mode;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = parameters.cull_mode == VK_CULL_MODE_FLAG_BITS_MAX_ENUM ? VK_CULL_MODE_BACK_BIT : parameters.cull_mode;
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
  if(parameters.blend_mode != BlendMode::None) {
    switch(parameters.blend_mode) {
      case BlendMode::Mult:
        color_blend_attachment.blendEnable = VK_TRUE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
        color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
        break;
      default: break;
    }
  } else if(_render_pass == &RenderPass::light_pass()) {
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  } else if(_render_pass == &RenderPass::present_pass()) {
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  }

  VkPipelineColorBlendAttachmentState color_blend_attachments[] {
    color_blend_attachment,
    color_blend_attachment,
  };

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
  color_blending.attachmentCount = _render_pass->color_attachment_count();
  color_blending.pAttachments = color_blend_attachments;

  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
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
    if(binding.name == name)
      return &binding;
  return nullptr;
}
