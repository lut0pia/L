#pragma once

#include <L/src/L.h>

namespace L {
  class GLSL : public Interface<GL::Program> {
    static GLSL instance;
    String _lib_vert, _lib_frag;
    static const size_t max_stage_count = 4;
    struct Stage {
      const char* start;
      GLenum type;
      size_t size;
      uint32_t line;
    };
  public:
    GLSL() : Interface{"glsl"},
      _lib_vert("#version 330 core\n" L_SHAREDUNIFORM "\n"),
      _lib_frag("#version 330 core\n" L_SHAREDUNIFORM L_SHADER_LIB "\n") {}
    Ref<GL::Program> from(const char* path) override {
      // Read file data
      CFileStream file_stream(path, "rb");
      if(!file_stream)
        return nullptr;
      const size_t size(file_stream.size());
      Buffer buffer(size);
      void* data(buffer.data());
      file_stream.read(data, size);

      const char* str((const char*)data);
      Stage stages[max_stage_count];
      uintptr_t stage(0);
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

      // Compile shaders
      byte shaders_mem[max_stage_count*sizeof(GL::Shader)];
      GL::Shader* shaders((GL::Shader*)shaders_mem);
      for(uintptr_t i(0); i<stage; i++) {
        const String& lib(stages[i].type==GL_FRAGMENT_SHADER ? _lib_frag : _lib_vert);
        char line_preprocessor[1024];
        sprintf(line_preprocessor, "#line %d \"%s\"\n", stages[i].line, path);
        const char* sources[] = {lib,line_preprocessor,stages[i].start};
        GLint source_lengths[] = {lib.size(),strlen(line_preprocessor),stages[i].size};
        new(shaders+i)GL::Shader(sources, source_lengths, L_COUNT_OF(sources), stages[i].type);
      }

      // Link program
      auto wtr(ref<GL::Program>(shaders, stage));

      for(uintptr_t i(0); i<stage; i++)
        shaders[i].~Shader();

      return wtr->check() ? wtr : nullptr;
    }
  };
  GLSL GLSL::instance;
}
