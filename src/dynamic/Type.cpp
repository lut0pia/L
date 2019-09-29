#include "Type.h"

#include "../image/Color.h"
#include "../macros.h"
#include "../text/String.h"
#include "../math/Vector.h"
#include "../stream/StringStream.h"
#include "../text/Symbol.h"
#include "../time/Time.h"

using namespace L;

Table<Symbol, const TypeDescription*> L::types;

void L::TypeInit() {
  // Operators
  Type<int8_t>::canall<>();
  Type<int16_t>::canall<>();
  Type<int32_t>::canall<>();
  Type<int64_t>::canall<>();
  Type<float>::canmath<>();
  Type<float>::cancmp<>();
  Type<double>::canmath<>();
  Type<double>::cancmp<>();
  Type<long double>::canmath<>();
  Type<long double>::cancmp<>();
  Type<String>::canadd<>();
  Type<String>::cancmp<>();
  Type<Symbol>::cancmp<>();
  Type<Time>::canmath<>();
  Type<Time>::cancmp<>();
  Type<Vector3f>::canmath();
  Type<float>::usemod([](void* a, const void* b) {*((float*)a) = pmod(*((float*)a), *((float*)b)); });

  // Unsigned types
  Type<uint8_t>::cancmp<>();
  Type<uint8_t>::canumath<>();
  Type<uint8_t>::canmod<>();
  Type<uint16_t>::cancmp<>();
  Type<uint16_t>::canumath<>();
  Type<uint16_t>::canmod<>();
  Type<uint32_t>::cancmp<>();
  Type<uint32_t>::canumath<>();
  Type<uint32_t>::canmod<>();
  Type<uint64_t>::cancmp<>();
  Type<uint64_t>::canumath<>();
  Type<uint64_t>::canmod<>();

  // Casts
  Type<void>::addcast<bool>([](void* dst, const void*) {new(dst)bool(false); });
  Type<int>::addcast<bool>([](void* dst, const void* src) {new(dst)bool((*(int*)src)!=0); });
  Type<int>::addcast<float>();
  Type<int>::addcast<String>([](void* dst, const void* src) {new(dst)String(ntos(*(int*)src)); });
  Type<unsigned int>::addcast<float>();
  Type<float>::addcast<int>();
  Type<float>::addcast<uint64_t>();
  Type<float>::addcast<String>([](void* dst, const void* src) {new(dst)String(ntos(*(float*)src)); });
  Type<float>::addcast<Vector3f>();
  Type<String>::addcast<bool>([](void* dst, const void* src) {new(dst)bool(!((String*)src)->empty()); });
  Type<String>::addcast<Color>([](void* dst, const void* src) {new(dst)Color(*(String*)src); });
  Type<String>::addcast<Symbol>([](void* dst, const void* src) {new(dst)Symbol((const char*)(*(String*)src)); });
  Type<Symbol>::addcast<String>([](void* dst, const void* src) {new(dst)String((const char*)(*(Symbol*)src)); });
  Type<Time>::addcast<float>([](void* dst, const void* src) {new(dst)float((*(Time*)src).fSeconds()); });
  Type<Time>::addcast<String>([](void* dst, const void* src) {new(dst)String(to_string(*(Time*)src)); });
}
