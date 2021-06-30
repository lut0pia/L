#include <L/src/container/Buffer.h>
#include <L/src/container/Ref.h>
#include <L/src/container/Table.h>
#include <L/src/dynamic/Variable.h>

#include <L/src/engine/Resource.inl>

L_PUSH_NO_WARNINGS

#include <json.h>

L_POP_NO_WARNINGS

using namespace L;

static const Symbol json_symbol = "json";

static void json_translate(Var& v, json_value_s* j) {
  if(json_string_s* json_string = json_value_as_string(j)) {
    v = String(json_string->string, json_string->string_size);
  } else if(json_number_s* json_number = json_value_as_number(j)) {
    v = float(atof(json_number->number));
  } else if(json_object_s* json_object = json_value_as_object(j)) {
    Table<Var, Var>& table = v.make<Ref<Table<Var, Var>>>().make();
    for(json_object_element_s* element = json_object->start; element->next != nullptr; element = element->next) {
      String key = String(element->name->string, element->name->string_size);
      Var& value = table[key];
      json_translate(value, element->value);
    }
  } else if(json_array_s* json_array = json_value_as_array(j)) {
    Array<Var>& array = v.make<Ref<Array<Var>>>().make();
    for(json_array_element_s* element = json_array->start; element->next != nullptr; element = element->next) {
      array.push();
      json_translate(array.back(), element->value);
    }
  } else if(json_value_is_true(j)) {
    v = true;
  } else if(json_value_is_false(j)) {
    v = false;
  } else if(json_value_is_null(j)) {
    v = nullptr;
  }
}
static bool json_loader(ResourceSlot& slot, Var::Intermediate& intermediate) {
  if(slot.ext != json_symbol) {
    return false;
  }

  Buffer buffer = slot.read_source_file();

  json_value_s* root = json_parse(buffer.data(), buffer.size());
  if(root == nullptr) {
    warning("json: Could not parse");
    return false;
  }

  json_translate(intermediate, root);

  free(root);

  return true;
}

void json_module_init() {
  ResourceLoading<Var>::add_loader(json_loader);
}
