#include "Cast.h"

using namespace L;
using namespace Dynamic;

Map<unsigned long long,CastFct> Cast::casts;

/*
    Precision order unsigned char < char < unsigned short < short
                    uint < int < unsigned long < long
                    unsigned long long < long long
                    float < double < long double
*/

void Cast::init(){
    #define TMP(T)  declare<T,bool>();\
                    declare<T,unsigned char>();\
                    declare<T,char>();\
                    declare<T,unsigned short>();\
                    declare<T,short>();\
                    declare<T,uint>();\
                    declare<T,int>();\
                    declare<T,unsigned long>();\
                    declare<T,long>();\
                    declare<T,unsigned long long>();\
                    declare<T,long long>();\
                    declare<T,float>();\
                    declare<T,double>();\
                    declare<T,long double>();

    TMP(bool)
    TMP(unsigned char)
    TMP(char)
    TMP(unsigned short)
    TMP(short)
    TMP(uint)
    TMP(int)
    TMP(unsigned long)
    TMP(long)
    TMP(unsigned long long)
    TMP(long long)
    TMP(float)
    TMP(double)
    TMP(long double)

    #undef TMP


    /*
    // Casts from unsigned char
    declare<unsigned char,char>();
    declare<unsigned char,unsigned short>();
    declare<unsigned char,short>();
    declare<unsigned char,uint>();
    declare<unsigned char,int>();
    declare<unsigned char,unsigned long>();
    declare<unsigned char,long>();
    declare<unsigned char,unsigned long long>();
    declare<unsigned char,long long>();
    declare<unsigned char,float>();
    declare<unsigned char,double>();
    declare<unsigned char,long double>();

    // Casts from char
    declare<char,unsigned short>();
    declare<char,short>();
    declare<char,uint>();
    declare<char,int>();
    declare<char,unsigned long>();
    declare<char,long>();
    declare<char,unsigned long long>();
    declare<char,long long>();
    declare<char,float>();
    declare<char,double>();
    declare<char,long double>();

    // Casts from unsigned short
    declare<unsigned short,short>();
    declare<unsigned short,uint>();
    declare<unsigned short,int>();
    declare<unsigned short,unsigned long>();
    declare<unsigned short,long>();
    declare<unsigned short,unsigned long long>();
    declare<unsigned short,long long>();
    declare<unsigned short,float>();
    declare<unsigned short,double>();
    declare<unsigned short,long double>();

    // Casts from short
    declare<short,uint>();
    declare<short,int>();
    declare<short,unsigned long>();
    declare<short,long>();
    declare<short,unsigned long long>();
    declare<short,long long>();
    declare<short,float>();
    declare<short,double>();
    declare<short,long double>();

    // Casts from uint
    declare<uint,int>();
    declare<uint,unsigned long>();
    declare<uint,long>();
    declare<uint,unsigned long long>();
    declare<uint,long long>();
    declare<uint,float>();
    declare<uint,double>();
    declare<uint,long double>();

    // Casts from int
    declare<int,unsigned long>();
    declare<int,long>();
    declare<int,unsigned long long>();
    declare<int,long long>();
    declare<int,float>();
    declare<int,double>();
    declare<int,long double>();

    // Casts from unsigned long
    declare<unsigned long,long>();
    declare<unsigned long,unsigned long long>();
    declare<unsigned long,long long>();
    declare<unsigned long,float>();
    declare<unsigned long,double>();
    declare<unsigned long,long double>();

    // Casts from long
    declare<long,unsigned long long>();
    declare<long,long long>();
    declare<long,float>();
    declare<long,double>();
    declare<long,long double>();

    // Casts from unsigned long long
    declare<unsigned long long,long long>();
    declare<unsigned long long,float>();
    declare<unsigned long long,double>();
    declare<unsigned long long,long double>();

    // Casts from long long
    declare<long long,float>();
    declare<long long,double>();
    declare<long long,long double>();

    // Casts from float
    declare<float,double>();
    declare<float,long double>();

    // Casts from double
    declare<double,long double>();
    */
}
