#ifndef DEF_L_AI_Perceptron
#define DEF_L_AI_Perceptron

#include <iostream>
#include "../stl.h"

namespace L{
    namespace AI{
        class Perceptron{
            private:
                size_t inputSize;
                float rate;
                Vector<float> weight;

            public:
                Perceptron(size_t inputSize, float rate);
                bool learn(const Vector<bool>& input, bool result);
                bool answer(const Vector<bool>& input);
        };
    }
}

#endif


