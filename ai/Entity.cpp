#include "Entity.h"

#include "../macros.h"
#include "../stl.h"

using namespace L;
using namespace AI;

Logic::Formula toFormula(const Vector<bool>& memory) {
  Logic::Formula wtr((size_t)0);
  if(!memory[0]) wtr = !wtr;
  for(size_t i=1; i<memory.size(); i++)
    wtr = wtr*((memory[i])?Logic::Formula(i):!Logic::Formula(i));
  return wtr;
}
Entity::Entity(size_t inputSize,size_t outputSize,size_t memorySlots)
  : inputSize(inputSize), outputSize(outputSize), memorySlots(memorySlots), inAverage(inputSize,0), currentTheory(outputSize) {
  // Precompute all subsets
  Vector<bool> v(outputSize,false);
  size_t i;
  do {
    i = 0;
    Set<size_t> subset;
    do v[i] = !v[i];
    while(!v[i] && ++i < outputSize);
    for(size_t j=0; j<outputSize; j++)
      if(v[i]) subset.insert(i);
    subsets.insert(subset);
  } while(i<outputSize);
}
Vector<bool> Entity::action(const Vector<bool>& input) {
  // Add new chunk to input memory
  inMemory.insert(inMemory.begin(),input.begin(),input.end());
  inMemory.resize(inputSize*memorySlots);
  // Add information to average of input bits
  for(size_t i=0; i<inAverage.size(); i++)
    inAverage[i] += ((input[i])?1:-1);
  // Compute results from current theory
  Vector<bool> output(currentTheory.size());
  for(size_t i=0; i<currentTheory.size(); i++)
    output[i] = currentTheory[i].interpret(inMemory);
  // Add new chunk to output memory
  outMemory.insert(outMemory.begin(),output.begin(),output.end());
  outMemory.resize(outputSize*memorySlots);
  return output;
}
void Entity::kill() {
  const Set<size_t>& subset(subsets.random());
  L_Iter(subset,it) { // For each formula to change
    if(outMemory[*it]) // AND NOT MEMORY
      currentTheory[*it] = currentTheory[*it]*(!toFormula(inMemory));
    else // OR MEMORY
      currentTheory[*it] = currentTheory[*it]+toFormula(inMemory);
    currentTheory[*it].simplify();
  }
}

void writeTheory(std::ostream& s,const Theory& theory) {
  s << '{';
  L_Iter(theory,it) {
    if(it!=theory.begin())
      s << ',';
    (*it).write(s);
  }
  s << '}';
}
void Entity::write(std::ostream& s) const {
  s << '{';
  writeTheory(s,currentTheory);
  /*
  L_Iter(theories,it){
      s << ',';
      writeTheory(s,*it);
  }*/
  s << '}';
}
void Entity::read(std::istream& s) {
}
