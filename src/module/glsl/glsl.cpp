#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Shader.h>
#include <L/src/rendering/shader_lib.h>
#include <L/src/system/System.h>
#include <L/src/text/String.h>
#include <L/src/stream/CFileStream.h>

using namespace L;

void glsl_loader(ResourceSlot& slot, Shader::Intermediate& intermediate) {
  intermediate.stage = VK_SHADER_STAGE_ALL;
  String stage_name;
  if(!strcmp(strrchr(slot.path, '.'), ".frag")) {
    intermediate.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stage_name = "frag";
  } else if(!strcmp(strrchr(slot.path, '.'), ".vert")) {
    intermediate.stage = VK_SHADER_STAGE_VERTEX_BIT;
    stage_name = "vert";
  }

  String cmd_output;
  char input_file[L_tmpnam], output_file[L_tmpnam];
  tmpnam(input_file);
  tmpnam(output_file);

  {
    Buffer original_text(CFileStream(slot.path, "rb").read_into_buffer());
    CFileStream input_stream(input_file, "wb");
    input_stream << L_GLSL_INTRO << L_SHAREDUNIFORM << L_PUSH_CONSTANTS;
    if(intermediate.stage == VK_SHADER_STAGE_FRAGMENT_BIT)
      input_stream << L_SHADER_LIB;
    input_stream << '\n';
    input_stream.write(original_text.data(), original_text.size());
  }

  const String cmd("glslangValidator -V -q -S "+stage_name+" "+input_file+" -o "+output_file);
  System::call(cmd, cmd_output);

  { // Parse debug information
    Shader::BindingType binding_type(Shader::BindingType::None);
    Array<String> lines(cmd_output.explode('\n')), words;
    for(String& line : lines) {
      if(line=="Uniform reflection:")
        binding_type = Shader::BindingType::Uniform;
      else if(line=="Uniform block reflection:")
        binding_type = Shader::BindingType::UniformBlock;
      else if(line=="Vertex attribute reflection:")
        binding_type = Shader::BindingType::VertexAttribute;
      else if(binding_type!=Shader::BindingType::None) {
        line.replaceAll(",", "").replaceAll(":", "");
        words = line.explode(' ');
        Shader::Binding binding;
        binding.name = Symbol(words[0]);
        binding.offset = atoi(words[2]);
        binding.type = binding_type;
        binding.size = atoi(words[6]);
        binding.index = atoi(words[8]);
        binding.binding = atoi(words[10]);
        binding.stage = intermediate.stage;

        // FIXME: this is a hack until we have working reflection for shader vertex attributes
        if(binding.name==Symbol("vposition")) {
          binding.binding = 0;
          binding.index = 0;
          binding.format = VK_FORMAT_R32G32B32_SFLOAT;
          binding.offset = 0;
          binding.size = 12;
        }
        if(binding.name==Symbol("vtexcoords")) {
          binding.binding = 0;
          binding.index = 1;
          binding.format = VK_FORMAT_R32G32_SFLOAT;
          binding.offset = 12;
          binding.size = 8;
        }
        if(binding.name==Symbol("vnormal")) {
          binding.binding = 0;
          binding.index = 2;
          binding.format = VK_FORMAT_R32G32B32_SFLOAT;
          binding.offset = 20;
          binding.size = 12;
        }
        if(binding.name==Symbol("vpositionfont")) {
          binding.binding = 0;
          binding.index = 0;
          binding.format = VK_FORMAT_R32G32_SFLOAT;
          binding.offset = 0;
          binding.size = 8;
        }
        if(binding.name==Symbol("vtexcoordsfont")) {
          binding.binding = 0;
          binding.index = 1;
          binding.format = VK_FORMAT_R32G32_SFLOAT;
          binding.offset = 8;
          binding.size = 8;
        }
        intermediate.bindings.push(binding);
      }
    }
  }

  intermediate.binary = CFileStream(output_file, "rb").read_into_buffer();
}

void glsl_module_init() {
  ResourceLoading<Shader>::add_loader("vert", glsl_loader);
  ResourceLoading<Shader>::add_loader("frag", glsl_loader);
}
