#pragma once

#include <L/src/L.h>

namespace L {
  class GLSL : public Interface<Program> {
    static GLSL instance;
    static const size_t max_stage_count = 4;
    struct Stage {
      const char* start;
      GLenum type;
      size_t size;
      uint32_t line;
    };
  public:
    GLSL() : Interface{"glsl"} {}
    Ref<Program> from(const uint8_t* data, size_t size) override {
      const char* str((const char*)data);
      Stage stages[max_stage_count];
      uintptr_t stage = 0;
      uint32_t line(1);
      bool newline(true);
      const char* pp_line(nullptr);
      for(uintptr_t i(0); i<size; i++) {
        char c(str[i]);
        if(newline) { // Start of line
          if(c=='#') // Start of a preprocessor line
            pp_line = str+i;
          newline = false;
        }
        if(c=='\n') { // End of line
          newline = true;
          line++;
          if(pp_line) { // End of a preprocessor line
            if(!memcmp(pp_line, "#stage ", 7)) { // Stage preprocessor
              // Close previous stage
              if(stage>0)
                stages[stage-1].size = pp_line-stages[stage-1].start;

              // Check new stage type
              if(!memcmp(pp_line+7, "vert", 4))
                stages[stage].type = GL_VERTEX_SHADER;
              else if(!memcmp(pp_line+7, "frag", 4))
                stages[stage].type = GL_FRAGMENT_SHADER;
              else return nullptr;
              stages[stage].start = str+i+1;
              stages[stage].line = line;
              stage++;
            }
            pp_line = nullptr;
          }
        }
      }
      // Close last stage if any
      if(stage>0)
        stages[stage-1].size = str+size-stages[stage-1].start;

      L_SCOPE_THREAD_MASK(1); // Go to main thread

      // Compile shaders
      uint8_t shaders_mem[max_stage_count*sizeof(Shader)];
      Shader* shaders((Shader*)shaders_mem);
      for(uintptr_t i(0); i<stage; i++) {
        static const char lib_vert[] = L_GLSL_INTRO L_SHAREDUNIFORM "\n";
        static const char lib_frag[] = L_GLSL_INTRO L_SHAREDUNIFORM L_SHADER_LIB "\n";
        const bool is_frag_shader(stages[i].type==GL_FRAGMENT_SHADER);
        const char* lib(is_frag_shader ? lib_frag : lib_vert);
        size_t lib_size((is_frag_shader ? sizeof(lib_frag) : sizeof(lib_vert))-1);
        char line_preprocessor[64];
        sprintf(line_preprocessor, "#line %d\n", stages[i].line);
        const char* sources[] = {lib,line_preprocessor,stages[i].start};
        GLint source_lengths[] = {GLint(lib_size),GLint(strlen(line_preprocessor)),GLint(stages[i].size)};
        new(shaders+i)Shader(sources, source_lengths, L_COUNT_OF(sources), stages[i].type);
      }

      // Link program
      Ref<Program> wtr(ref<Program>(shaders, stage));

      // Destruct shaders
      for(uintptr_t i(0); i<stage; i++)
        shaders[i].~Shader();
      return wtr->check() ? wtr : nullptr;
    }
  };
  GLSL GLSL::instance;
}
