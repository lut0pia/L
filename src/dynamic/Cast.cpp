#include "Cast.h"

using namespace L;

Map<uint64_t,CastFct> Cast::casts;

void Cast::init() {
  // T -> T
#define TMP(T)\
  do{\
    declare<T,unsigned char>();\
    declare<T,char>();\
    declare<T,unsigned short>();\
    declare<T,short>();\
    declare<T,uint32_t>();\
    declare<T,int>();\
    declare<T,unsigned long>();\
    declare<T,long>();\
    declare<T,unsigned long long>();\
    declare<T,long long>();\
    declare<T,float>();\
    declare<T,double>();\
    declare<T,long double>();\
  }while(false)
  TMP(bool);
  TMP(unsigned char);
  TMP(char);
  TMP(unsigned short);
  TMP(short);
  TMP(uint32_t);
  TMP(int);
  TMP(unsigned long);
  TMP(long);
  TMP(unsigned long long);
  TMP(long long);
  TMP(float);
  TMP(double);
  TMP(long double);
#undef TMP
   // T -> bool
#define TMP(T)\
  declare<T,bool>([](const Var& a,Var& b){\
    b = a.as<T>()!=0;\
  })
  TMP(unsigned char);
  TMP(unsigned short);
  TMP(short);
  TMP(uint32_t);
  TMP(int);
  TMP(unsigned long);
  TMP(long);
  TMP(unsigned long long);
  TMP(long long);
  TMP(float);
  TMP(double);
  TMP(long double);
#undef TMP
  declare<char,bool>([](const Var& a,Var& b){
    b = a.as<char>()!='\0';
  });
  declare<String,bool>([](const Var& a,Var& b){
    b = !a.as<String>().empty();
  });
}
