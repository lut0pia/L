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
    struct TaskPayload {
      Stage stages[max_stage_count];
      Ref<Program> wtr;
      uintptr_t stage = 0;
    };
  public:
    GLSL() : Interface{"glsl"} {}
    Ref<Program> from(const byte* data, size_t size) override {
      const char* str((const char*)data);
      TaskPayload payload;
      Stage* stages(payload.stages);
      uintptr_t& stage(payload.stage);
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

      TaskSystem::push([](void* p) {
        TaskPayload& payload(*(TaskPayload*)p);

        // Compile shaders
        byte shaders_mem[max_stage_count*sizeof(Shader)];
        Shader* shaders((Shader*)shaders_mem);
        for(uintptr_t i(0); i<payload.stage; i++) {
          static const char lib_vert[] = L_GLSL_INTRO L_SHAREDUNIFORM "\n";
          static const char lib_frag[] = L_GLSL_INTRO L_SHAREDUNIFORM L_SHADER_LIB "\n";
          const bool is_frag_shader(payload.stages[i].type==GL_FRAGMENT_SHADER);
          const char* lib(is_frag_shader ? lib_frag : lib_vert);
          size_t lib_size((is_frag_shader ? sizeof(lib_frag) : sizeof(lib_vert))-1);
          char line_preprocessor[64];
          sprintf(line_preprocessor, "#line %d\n", payload.stages[i].line);
          const char* sources[] = {lib,line_preprocessor,payload.stages[i].start};
          GLint source_lengths[] = {GLint(lib_size),GLint(strlen(line_preprocessor)),GLint(payload.stages[i].size)};
          new(shaders+i)Shader(sources, source_lengths, L_COUNT_OF(sources), payload.stages[i].type);
        }

        // Link program
        payload.wtr = ref<Program>(shaders, payload.stage);

        // Destruct shaders
        for(uintptr_t i(0); i<payload.stage; i++)
          shaders[i].~Shader();

        // Don't return ill-formed programs
        if(!payload.wtr->check())
          payload.wtr = nullptr;
      }, &payload, TaskSystem::MainThread);
      TaskSystem::join();
      return payload.wtr;
    }
  };
  GLSL GLSL::instance;
}
