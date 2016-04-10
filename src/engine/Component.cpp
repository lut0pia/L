#include "Component.h"

using namespace L;

Component::~Component(){
  entity()->remove(this);
}