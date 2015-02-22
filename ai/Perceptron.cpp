#include "Perceptron.h"

using namespace L;
using namespace AI;

Perceptron::Perceptron(size_t is, float r)
: inputSize(is), rate(r), weight(inputSize,0.0f){}

bool Perceptron::learn(const Vector<bool>& input, bool result){
    if(answer(input) != result){
        float mod((result)?rate:-rate);
        for(size_t i(0);i<inputSize;i++)
            if(input[i])
                weight[i] += mod;
        return true;
    }
    else return false;
}
bool Perceptron::answer(const Vector<bool>& input){
    float sum(0);
    for(size_t i(0);i<inputSize;i++)
        sum += ((input[i])?1:0)*weight[i];
    return (sum>0);
}
