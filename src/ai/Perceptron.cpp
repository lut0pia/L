#include "Perceptron.h"

using namespace L;
using namespace AI;

Perceptron::Perceptron(int is, float r)
  : inputSize(is), rate(r), weight(inputSize,0.0f) {}

bool Perceptron::learn(const Array<bool>& input, bool result) {
  if(answer(input) != result) {
    float mod((result)?rate:-rate);
    for(int i(0); i<inputSize; i++)
      if(input[i])
        weight[i] += mod;
    return true;
  } else return false;
}
bool Perceptron::answer(const Array<bool>& input) {
  float sum(0);
  for(int i(0); i<inputSize; i++)
    sum += ((input[i])?1:0)*weight[i];
  return (sum>0);
}
