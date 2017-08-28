#pragma once

#include <L/src/L.h>

namespace L {
  class MaterialLON : public Interface<Material> {
    static MaterialLON instance;
  public:
    MaterialLON() : Interface{"lon"} {}
    Ref<Material> from(Stream& stream) override {
      Script::Context context;
      Var data(context.executeInside(Array<Var>{ref<Script::CodeFunction>(Script::Context::read(stream))}));

      if(data.is<Ref<Table<Var, Var>>>()) {
        static const Symbol program_symbol("program"), textures_symbol("textures");
        Table<Var, Var>& table(*data.as<Ref<Table<Var, Var>>>());
        Var *program_var(table.find(program_symbol)), *textures_var(table.find(textures_symbol));
        if(program_var) {
          auto wtr(ref<Material>());
          wtr->program = Resource<GL::Program>::get(program_var->get<String>());
          if(textures_var && textures_var->is<Ref<Table<Var, Var>>>()) {
            Table<Var, Var>& textures(*textures_var->as<Ref<Table<Var, Var>>>());
            for(auto& texture : textures)
              wtr->textures.push(
                Symbol(texture.key().get<String>()),
                Resource<GL::Texture>::get(texture.value().get<String>()));
          }
          return wtr;
        }
      }
      return nullptr;
    }
  };
  MaterialLON MaterialLON::instance;
}
