#ifndef DEF_L_Rand
#define DEF_L_Rand

#include "types.h"

namespace L{
    class Rand{
        private:
            static const ullong a, b;
            static ullong last;

        public:
            static ullong next();
            static byte nextByte();
            static double nextDouble();

            static ullong next(ullong min, ullong max); // Returns a random unsigned integer between min and max
            static int next(int min, int max); // Returns a random integer between min and max
            static double next(double min, double max); // Returns a random double between min and max

            static double gauss(uint);
            static ullong next(ullong ave); // Random exp with an average n

            inline static const ullong& gState(){return last;}
            inline static void sState(const ullong& s){last = s;}
    };
}

#endif

