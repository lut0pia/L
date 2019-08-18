#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/pipeline/ShaderTools.h>
#include <L/src/rendering/Shader.h>
#include <L/src/rendering/shader_lib.h>
#include <L/src/system/System.h>
#include <L/src/text/String.h>
#include <L/src/stream/CFileStream.h>

using namespace L;

bool glsl_loader(ResourceSlot& slot, Shader::Intermediate& intermediate) {
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

#ifdef L_WINDOWS
#define GLSL_VALIDATOR "%VULKAN_SDK%\\Bin\\glslangValidator"
#else
#define GLSL_VALIDATOR "glslangValidator"
#endif
  const String cmd(GLSL_VALIDATOR " -V -S " + stage_name + " " + input_file + " -o " + output_file);
  const int cmd_result(System::call(cmd, cmd_output));

  if(cmd_result != 0 || cmd_output.empty()) {
    return false;
  }

  { // Parse debug information
    Array<String> lines(cmd_output.explode('\n')), words;

    { // Early exit in case of errors
      bool has_errors(false);
      for(String& line : lines) {
        if(strstr(line, "ERROR:")) {
          line.replaceAll(input_file, String(slot.path));
          err << line << '\n';
          has_errors = true;
        }
      }

      if(has_errors) {
        return false;
      }
    }
  }

  if(CFileStream file_stream = CFileStream(output_file, "rb")) {
    intermediate.binary = file_stream.read_into_buffer();
    ShaderTools::reflect(intermediate);
    return true;
  } else {
    return false;
  }
}

void glsl_module_init() {
  ResourceLoading<Shader>::add_loader("vert", glsl_loader);
  ResourceLoading<Shader>::add_loader("frag", glsl_loader);
}
