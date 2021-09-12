#include <L/src/container/Buffer.h>
#include <L/src/container/Ref.h>
#include <L/src/container/Table.h>
#include <L/src/dynamic/Variable.h>

#include <L/src/engine/Resource.inl>

L_PUSH_NO_WARNINGS

#define YAML_DECLARE_STATIC
#include <yaml.h>

L_POP_NO_WARNINGS

using namespace L;

static const Symbol yaml_symbol = "yaml";

static void yaml_translate(Var& v, yaml_node_t* nodes, yaml_node_t* y) {
  switch(y->type) {
    case yaml_node_type_e::YAML_NO_NODE: break;
    case yaml_node_type_e::YAML_SCALAR_NODE:
      v = String((const char*)y->data.scalar.value, y->data.scalar.length);
      break;
    case yaml_node_type_e::YAML_SEQUENCE_NODE: {
      Array<Var>& array = v.make<Ref<Array<Var>>>().make();
      for(yaml_node_item_t* child = y->data.sequence.items.start; child < y->data.sequence.items.end; child++) {
        array.push();
        yaml_translate(array.back(), nodes, nodes + *child);
      }
    } break;
    case yaml_node_type_e::YAML_MAPPING_NODE: {
      Table<Var, Var>& table = v.make<Ref<Table<Var, Var>>>().make();
      for(yaml_node_pair_t* pair = y->data.mapping.pairs.start; pair < y->data.mapping.pairs.end; pair++) {
        Var key;
        yaml_translate(key, nodes, nodes + pair->key);
        yaml_translate(table[key], nodes, nodes + pair->value);
      }
    } break;
  }
}
static bool yaml_loader(ResourceSlot& slot, Var::Intermediate& intermediate) {
  if(slot.ext != yaml_symbol) {
    return false;
  }

  Buffer buffer = slot.read_source_file();
  yaml_parser_t parser;
  yaml_parser_initialize(&parser);
  yaml_parser_set_input_string(&parser, (const unsigned char*)buffer.data(), buffer.size());

  yaml_document_t document;

  if(!yaml_parser_load(&parser, &document)) {
    yaml_parser_delete(&parser);
    warning("yaml: Could not load %s", (const char*)slot.id);
    return false;
  }

  yaml_translate(intermediate, document.nodes.start - 1, yaml_document_get_root_node(&document));

  yaml_parser_delete(&parser);

  return true;
}

void yaml_module_init() {
  ResourceLoading<Var>::add_loader(yaml_loader);
}
