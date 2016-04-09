#ifndef DEF_L_AI_MLP
#define DEF_L_AI_MLP

#include "../math/Vector.h"

namespace L {
  namespace AI {
    template <size_t insize, size_t outsize, size_t hisize=insize*outsize>
    class MLP {
      private:
        float _rate, _momentum,
              _inputWeight[insize+1][hisize], _hiddenWeight[hisize+1][outsize],
              _inputWeightDelta[insize+1][hisize], _hiddenWeightDelta[hisize+1][outsize];
      public:
        inline MLP(float rate = .5f, float momentum = .5f) : _rate(rate), _momentum(momentum) {
          reset();
        }
        inline float rate() const {return _rate;}
        inline void rate(float rate) {_rate = rate;}
        inline float momentum() const {return _momentum;}
        inline void momentum(float momentum) {_momentum = momentum;}
        inline float inputWeight(int i, int j) const  {return _inputWeight[i][j];}
        inline float hiddenWeight(int i, int j) const {return _hiddenWeight[i][j];}
        void reset() {
          for(int i(0); i<=insize; i++)
            for(int j(0); j<hisize; j++) {
              _inputWeight[i][j] = Rand::nextFloat()-.5f;
              _inputWeightDelta[i][j] = 0;
            }
          for(int i(0); i<=hisize; i++)
            for(int j(0); j<outsize; j++) {
              _hiddenWeight[i][j] = Rand::nextFloat()-.5f;
              _hiddenWeightDelta[i][j] = 0;
            }
        }
        inline Vector<outsize,float> answer(const Vector<insize,float>& input) const {
          Vector<hisize,float> hidden;
          return answer(input,hidden);
        }
        Vector<outsize,float> answer(const Vector<insize,float>& input, Vector<hisize,float>& hidden) const {
          Vector<outsize,float> output;
          for(int i(0); i<hisize; i++) {
            hidden[i] = 0;
            for(int j(0); j<=insize; j++)
              hidden[i] += ((j<insize)?input[j]:-1.f) * _inputWeight[j][i];
            hidden[i] = activation(hidden[i]);
          }
          for(int i(0); i<outsize; i++) {
            output[i] = 0;
            for(int j(0); j<=hisize; j++)
              output[i] += ((j<hisize)?hidden[j]:-1.f) * _hiddenWeight[j][i];
            output[i] = activation(output[i]);
          }
          return output;
        }
        float learn(const Vector<insize,float>& input, const Vector<outsize,float>& desired) {
          Vector<hisize,float> hidden, hiddenError;
          Vector<outsize,float> output(answer(input,hidden)), outputError;
          // Compute output errors
          for(int i(0); i<outsize; i++) {
            outputError[i] = output[i]*(1.f-output[i])*(desired[i]-output[i]); // Compute output error
            for(int j(0); j<=hisize; j++)
              _hiddenWeightDelta[j][i] = _rate * hidden[j] * outputError[i] + _momentum * _hiddenWeightDelta[j][i];
          }
          // Compute hidden errors
          for(int i(0); i<hisize; i++) {
            hiddenError[i] = 0;
            for(int j(0); j<outsize; j++)
              hiddenError[i] += _hiddenWeight[i][j] * outputError[j];
            hiddenError[i] = hidden[i]*(1.f-hidden[i])*hiddenError[i];
            for(int j(0); j<=insize; j++)
              _inputWeightDelta[j][i] = _rate * input[j] * hiddenError[i] + _momentum * _inputWeightDelta[j][i];
          }
          // Apply weight delta
          for(int i(0); i<=hisize; i++)
            for(int j(0); j<outsize; j++)
              _hiddenWeight[i][j] += _hiddenWeightDelta[i][j];
          for(int i(0); i<=insize; i++)
            for(int j(0); j<hisize; j++)
              _inputWeight[i][j] += _inputWeightDelta[i][j];
          return (desired-output).abs().sum();
        }
        inline static float activation(float v) {
          return 1.f/(1.f+exp(-v)); // Sigmoid
        }
    };
  }
  template <size_t insize, size_t outsize, size_t hisize=insize*outsize>
  Stream& operator<<(Stream& s, const AI::MLP<insize,outsize,hisize>& v) {
    s << '(' << v.rate() << ';' << v.momentum();
    for(int i(0); i<=hisize; i++)
      for(int j(0); j<outsize; j++)
        s << ';' << v.hiddenWeight(i,j);
    for(int i(0); i<=insize; i++)
      for(int j(0); j<hisize; j++)
        s << ';' << v.inputWeight(i,j);
    s << ')';
    return s;
  }
}

#endif


