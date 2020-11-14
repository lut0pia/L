#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Shader.h>
#include <L/src/dev/profiling.h>

using namespace L;

// https://www.khronos.org/registry/spir-v/specs/1.0/SPIRV.html

const uint32_t
OpName(5),
OpMemberName(6),
OpEntryPoint(15),
OpTypeInt(21),
OpTypeFloat(22),
OpTypeVector(23),
OpTypeStruct(30),
OpTypePointer(32),
OpVariable(59),
OpDecorate(71),
OpMemberDecorate(72),
DeLocation(30),
DeIndex(32),
DeBinding(33),
DeOffset(35);

static uint32_t read_opcode(const uint32_t* ptr) { return *ptr & 0xffff; }
static uint32_t read_word_count(const uint32_t* ptr) { return (*ptr >> 16) & 0xffff; }
static const uint32_t* find_opcode(const uint32_t* binary, uint32_t opcode) {
  binary += 5; // Skip header
  while(read_opcode(binary) != opcode) {
    binary += read_word_count(binary);
  }
  return binary;
}
static const uint32_t* find_id(const uint32_t* binary, size_t size, uint32_t id) {
  const uint32_t* end(binary + (size / 4));
  binary += 5; // Skip header
  uint32_t current_id(~0u);
  while(binary < end) {
    switch(read_opcode(binary)) {
      case OpTypeStruct:
      case OpTypeInt:
      case OpTypeFloat:
      case OpTypeVector:
      case OpTypePointer: current_id = binary[1]; break;
      case OpVariable: current_id = binary[2]; break;
    }
    if(current_id == id) {
      return binary;
    }
    binary += read_word_count(binary);
  }
  return nullptr;
}
template <class F>
static void for_opcodes(const uint32_t* binary, size_t size, uint32_t opcode, F callback) {
  const uint32_t* end(binary + (size / 4));
  binary += 5; // Skip header
  while(binary < end) {
    if(read_opcode(binary) == opcode) {
      callback(binary);
    }
    binary += read_word_count(binary);
  }
}
static RenderFormat find_type(const uint32_t* binary, size_t size, uint32_t type_id) {
  const uint32_t* type(find_id(binary, size, type_id));
  if(type != nullptr) {
    switch(read_opcode(type)) {
      case OpTypeInt:
        switch(type[2]) {
          case 16: return RenderFormat::R16_UInt;
          case 32: return RenderFormat::R32_UInt;
          default: break;
        }
        break;
      case OpTypeFloat:
        switch(type[2]) {
          case 16: return RenderFormat::R16_SFloat;
          case 32: return RenderFormat::R32_SFloat;
          default: break;
        }
        break;
      case OpTypeVector:
        switch(find_type(binary, size, type[2])) {
          case RenderFormat::R16_SInt:
            switch(type[3]) {
              case 2: return RenderFormat::R16G16_SInt;
              case 3: return RenderFormat::R16G16B16_SInt;
              case 4: return RenderFormat::R16G16B16A16_SInt;
              default: break;
            }
            break;
          case RenderFormat::R16_UInt:
            switch(type[3]) {
              case 2: return RenderFormat::R16G16_UInt;
              case 3: return RenderFormat::R16G16B16_UInt;
              case 4: return RenderFormat::R16G16B16A16_UInt;
              default: break;
            }
            break;
          case RenderFormat::R16_SFloat:
            switch(type[3]) {
              case 2: return RenderFormat::R16G16_SFloat;
              case 3: return RenderFormat::R16G16B16_SFloat;
              case 4: return RenderFormat::R16G16B16A16_SFloat;
              default: break;
            }
            break;
          case RenderFormat::R32_SInt:
            switch(type[3]) {
              case 2: return RenderFormat::R32G32_SInt;
              case 3: return RenderFormat::R32G32B32_SInt;
              case 4: return RenderFormat::R32G32B32A32_SInt;
              default: break;
            }
            break;
          case RenderFormat::R32_UInt:
            switch(type[3]) {
              case 2: return RenderFormat::R32G32_UInt;
              case 3: return RenderFormat::R32G32B32_UInt;
              case 4: return RenderFormat::R32G32B32A32_UInt;
              default: break;
            }
            break;
          case RenderFormat::R32_SFloat:
            switch(type[3]) {
              case 2: return RenderFormat::R32G32_SFloat;
              case 3: return RenderFormat::R32G32B32_SFloat;
              case 4: return RenderFormat::R32G32B32A32_SFloat;
              default: break;
            }
            break;
          default: break;
        }
        break;
      case OpTypePointer: return find_type(binary, size, type[3]);
      default: break;
    }
  }
  return RenderFormat::Undefined;
}
static size_t find_type_size(const uint32_t* binary, size_t size, uint32_t type_id) {
  RenderFormat type_format(find_type(binary, size, type_id));
  if(size_t type_size = Renderer::format_size(type_format)) {
    return type_size;
  } else if(const uint32_t* type = find_id(binary, size, type_id)) {
    switch(read_opcode(type)) {
      case OpTypeStruct:
      {
        const uint32_t struct_member_count(read_word_count(type) - 2);
        const uint32_t last_member_type_id(type[struct_member_count + 1]);
        const size_t last_member_size(find_type_size(binary, size, last_member_type_id));
        uint32_t last_member_offset(0);
        for_opcodes(binary, size, OpMemberDecorate, [&](const uint32_t* decoration) {
          if(decoration[1] == type_id && decoration[2] == struct_member_count - 1 && decoration[3] == DeOffset) {
            last_member_offset = decoration[4];
          }
        });
        return last_member_offset + last_member_size;
      }
      break;
    }
  }
  return 0;
}
static void shader_reflect(const ResourceSlot&, Shader::Intermediate& intermediate) {
  L_SCOPE_MARKER("SPIR-V reflection");
  const uint32_t* binary((const uint32_t*)intermediate.binary.data());
  const size_t size(intermediate.binary.size());
  const uint32_t* execution_mode(find_opcode(binary, OpEntryPoint));
  switch(execution_mode[1]) {
    case 0: intermediate.stage = ShaderStage::Vertex; break;
    case 3: intermediate.stage = ShaderStage::Geometry; break;
    case 4: intermediate.stage = ShaderStage::Fragment; break;
    default: error("Couldn't determine shader stage from SPIR-V");
  }

  for_opcodes(binary, size, OpVariable, [&](const uint32_t* variable) {
    const uint32_t variable_id(variable[2]);
    const uint32_t* type_pointer(find_id(binary, size, variable[1]));
    const uint32_t type_id(type_pointer[3]);
    ShaderBinding binding {};

    for_opcodes(binary, size, OpName, [&](const uint32_t* name) {
      if((!binding.name && name[1] == variable_id) || name[1] == type_id) {
        binding.name = (const char*)(name + 2);
      }
    });

    switch(variable[3]) {
      case 0: binding.type = ShaderBindingType::UniformConstant; break;
      case 1: binding.type = ShaderBindingType::Input; break;
      case 2: binding.type = ShaderBindingType::Uniform; break;
      case 9: binding.type = ShaderBindingType::PushConstant; break;
      default: return;
    }

    binding.stage = intermediate.stage;
    binding.format = find_type(binary, size, type_id);
    binding.size = int32_t(find_type_size(binary, size, type_id));

    for_opcodes(binary, size, OpDecorate, [&](const uint32_t* decoration) {
      if(decoration[1] == variable_id) {
        switch(decoration[2]) {
          case DeIndex:
          case DeLocation: binding.index = decoration[3]; break;
          case DeBinding: binding.binding = decoration[3]; break;
          case DeOffset: binding.offset = decoration[3]; break;
        }
      }
    });
    intermediate.bindings.push(binding);
    if(binding.type == ShaderBindingType::Uniform) { // Create bindings for every member of block
      binding.type = ShaderBindingType::Undefined;
      for_opcodes(binary, size, OpMemberDecorate, [&](const uint32_t* decoration) {
        if(decoration[1] == type_id) {
          binding.index = decoration[2];
          switch(decoration[3]) {
            case DeOffset: binding.offset = decoration[4]; break;
            default: return;
          }
          for_opcodes(binary, size, OpMemberName, [&](const uint32_t* name) {
            if(name[1] == type_id && int32_t(name[2]) == binding.index) {
              binding.name = (const char*)(name + 3);
            }
          });
          intermediate.bindings.push(binding);
        }
      });
    }
  });

  for(uintptr_t i(0); i < intermediate.bindings.size(); i++) {
    if(intermediate.bindings[i].name == Symbol("gl_FragCoord") ||
      intermediate.bindings[i].name == Symbol("gl_VertexIndex")) {
      intermediate.bindings.erase_fast(i--);
    } else if(intermediate.bindings[i].type == ShaderBindingType::Input) {
      // Compute Input offset by checking all Input with lesser index
      ShaderBinding& binding(intermediate.bindings[i]);
      for(const ShaderBinding& other_binding : intermediate.bindings) {
        if(other_binding.type == ShaderBindingType::Input && other_binding.index < binding.index) {
          binding.offset += other_binding.size;
        }
      }
    }
  }
}

void shader_reflect_module_init() {
  ResourceLoading<Shader>::add_transformer(shader_reflect);
}
