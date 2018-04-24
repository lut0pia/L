#pragma once

#include <L/src/L.h>

namespace L {
  class MaterialLON : public Interface<Material> {
    static MaterialLON instance;
  public:
    MaterialLON() : Interface{"lon"} {}
    Ref<Material> from(const uint8_t* str, size_t size) override {
      Var data;
      {
        Script::Compiler compiler;
        Script::Context context;
        compiler.read((const char*)str, size, true);
        data = context.executeInside(Array<Var>{ref<Script::CodeFunction>(compiler.function())});
      }

      if(data.is<Ref<Table<Var, Var>>>()) {
        static const Symbol program_symbol("program"),
          scalars_symbol("scalars"), textures_symbol("textures"), vectors_symbol("vectors"),
          vertex_count_symbol("vertex_count"), primitive_mode_symbol("primitive_mode"),
          triangles_symbol("triangles");
        Table<Var, Var>& table(*data.as<Ref<Table<Var, Var>>>());
        Var *program_var(table.find(program_symbol)),
          *scalars_var(table.find(scalars_symbol)), *textures_var(table.find(textures_symbol)), *vectors_var(table.find(vectors_symbol)),
          *vertex_count_var(table.find(vertex_count_symbol)), *primitive_mode_var(table.find(primitive_mode_symbol));
        if(program_var) {
          auto wtr(ref<Material>());
          wtr->program(Resource<Program>::get(program_var->get<String>()));
          // Iterate over scalars
          if(scalars_var && scalars_var->is<Ref<Table<Var, Var>>>())
            for(const auto& scalar : *scalars_var->as<Ref<Table<Var, Var>>>())
              wtr->scalar(scalar.key().get<Symbol>(), scalar.value().get<float>());
          // Iterate over textures
          if(textures_var && textures_var->is<Ref<Table<Var, Var>>>())
            for(const auto& texture : *textures_var->as<Ref<Table<Var, Var>>>())
              wtr->texture(texture.key().get<Symbol>(), Resource<Texture>::get(texture.value().get<String>()));
          // Iterate over vectors
          if(vectors_var && vectors_var->is<Ref<Table<Var, Var>>>())
            for(const auto& vector : *vectors_var->as<Ref<Table<Var, Var>>>())
              if(vector.value().canbe<Vector4f>())
                wtr->vector(vector.key().get<Symbol>(), vector.value().get<Vector4f>());
              else
                wtr->color(vector.key().get<Symbol>(), vector.value().get<Color>());

          if(vertex_count_var)
            wtr->vertex_count(vertex_count_var->get<int>());
          if(primitive_mode_var) {
            const Symbol primitive_mode(primitive_mode_var->get<Symbol>());
            if(primitive_mode==triangles_symbol)
              wtr->primitive_mode(GL_TRIANGLES);
          }
          return wtr;
        }
      }
      return nullptr;
    }
  };
  MaterialLON MaterialLON::instance;
}
