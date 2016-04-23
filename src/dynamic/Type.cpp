#include "Type.h"

#include "../macros.h"
#include "../String.h"

using namespace L;

Cast TypeDescription::cast(const TypeDescription* target) const{
  const KeyValue<intptr_t,Cast>* it(casts.find((intptr_t)target));
  L_ASSERT(it);
  return (it) ? it->value() : nullptr;
}

void L::TypeInit(){
  // Operators
  Type<uint8_t>::canall<>();
  Type<int8_t>::canall<>();
  Type<uint16_t>::canall<>();
  Type<int16_t>::canall<>();
  Type<uint32_t>::canall<>();
  Type<int32_t>::canall<>();
  Type<uint64_t>::canall<>();
  Type<int64_t>::canall<>();
  Type<float>::canmath<>();
  Type<float>::cancmp<>();
  Type<double>::canmath<>();
  Type<double>::cancmp<>();
  Type<long double>::canmath<>();
  Type<long double>::cancmp<>();
  Type<String>::canadd<>();
  Type<String>::cancmp<>();

  // Casts
  Type<int>::addcast<bool>([](void* dst,const void* src){new(dst)bool((*(int*)src)!=0); });
  Type<int>::addcast<float>();
  Type<int>::addcast<String>([](void* dst,const void* src){new(dst)String(ntos(*(int*)src)); });
  Type<float>::addcast<int>();
  Type<String>::addcast<bool>([](void* dst,const void* src){new(dst)bool(!((String*)src)->empty()); });
}