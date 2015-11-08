#include "encoding.h"

#include "../tmp.h"

using namespace L;

const char* L::UTF16toUTF8(uint16 utf16) {
  static char wtr[4];
  if(utf16<static_pow<2,7>::value) {
    wtr[0] = utf16;
    wtr[1] = 0;
  } else if(utf16<static_pow<2,11>::value) {
    wtr[0] = 0xC0 | (utf16>>6);
    wtr[1] = 0x80 | (utf16 & bitmask<6>::value);
    wtr[2] = 0;
  } else {
    wtr[0] = 0xE0 | (utf16>>12);
    wtr[1] = 0x80 | ((utf16>>6) & bitmask<6>::value);
    wtr[2] = 0x80 | (utf16 & bitmask<6>::value);
    wtr[3] = 0;
  }
  return wtr;
}
uint32 L::UTF8toUTF32(const char* str, int* size) {
  uint32 wtr((byte)*str);
  if(wtr>>5==0x6) { // 2 bytes
    wtr <<= 6;
    wtr |= ((byte)*(++str)) & bitmask<6>::value;
    wtr &= bitmask<11>::value;
    *size = 2;
  } else if(wtr>>4==0xE) { // 3 bytes
    wtr <<= 6;
    wtr |= ((byte)*(++str)) & bitmask<6>::value;
    wtr <<= 6;
    wtr |= ((byte)*(++str)) & bitmask<6>::value;
    wtr &= bitmask<16>::value;
    *size = 3;
  } else *size = 1;
  return wtr;
}
Array<uint32> L::UTF8toUTF32(const char* str) {
  Array<uint32> wtr;
  int utfsize;
  while(*str) {
    wtr.push(UTF8toUTF32(str,&utfsize));
    str+=utfsize;
  }
  return wtr;
}
String L::ANSItoUTF8(String str) {
#define ATU(a,u) case a: \
    str.replace(i,1,u); \
    i += String(u).size()-1; \
    break;
  for(size_t i(0); i<str.size(); i++) {
    switch((byte)str[i]) {
        ATU(0x80,"€")               ATU(0x82,"‚") ATU(0x83,"ƒ") ATU(0x84,"„") ATU(0x85,"…") ATU(0x86,"†") ATU(0x87,"‡")
        ATU(0x88,"ˆ") ATU(0x89,"‰") ATU(0x8A,"Š") ATU(0x8B,"‹") ATU(0x8C,"Œ")               ATU(0x8E,"Ž")
        ATU(0x91,"‘") ATU(0x92,"’") ATU(0x93,"“") ATU(0x94,"”") ATU(0x95,"•") ATU(0x96,"–") ATU(0x97,"—")
        ATU(0x98,"~") ATU(0x99,"™") ATU(0x9A,"š") ATU(0x9B,"›") ATU(0x9C,"œ")               ATU(0x9E,"ž") ATU(0x9F,"Ÿ")
        ATU(0xA0," ") ATU(0xA1,"¡") ATU(0xA2,"¢") ATU(0xA3,"£") ATU(0xA4,"¤") ATU(0xA5,"¥") ATU(0xA6,"¦") ATU(0xA7,"§") // A0 NBSP
        ATU(0xA8,"¨") ATU(0xA9,"©") ATU(0xAA,"ª") ATU(0xAB,"«") ATU(0xAC,"¬") ATU(0xAD,"-") ATU(0xAE,"®") ATU(0xAF,"¯") // AD soft hyphen
        ATU(0xB0,"°") ATU(0xB1,"±") ATU(0xB2,"²") ATU(0xB3,"³") ATU(0xB4,"´") ATU(0xB5,"µ") ATU(0xB6,"¶") ATU(0xB7,"·")
        ATU(0xB8,"¸") ATU(0xB9,"¹") ATU(0xBA,"º") ATU(0xBB,"»") ATU(0xBC,"¼") ATU(0xBD,"½") ATU(0xBE,"¾") ATU(0xBF,"¿")
        ATU(0xC0,"À") ATU(0xC1,"Á") ATU(0xC2,"Â") ATU(0xC3,"Ã") ATU(0xC4,"Ä") ATU(0xC5,"Å") ATU(0xC6,"Æ") ATU(0xC7,"Ç")
        ATU(0xC8,"È") ATU(0xC9,"É") ATU(0xCA,"Ê") ATU(0xCB,"Ë") ATU(0xCC,"Ì") ATU(0xCD,"Í") ATU(0xCE,"Î") ATU(0xCF,"Ï")
        ATU(0xD0,"Ð") ATU(0xD1,"Ñ") ATU(0xD2,"Ò") ATU(0xD3,"Ó") ATU(0xD4,"Ô") ATU(0xD5,"Õ") ATU(0xD6,"Ö") ATU(0xD7,"×")
        ATU(0xD8,"Ø") ATU(0xD9,"Ù") ATU(0xDA,"Ú") ATU(0xDB,"Û") ATU(0xDC,"Ü") ATU(0xDD,"Ý") ATU(0xDE,"Þ") ATU(0xDF,"ß")
        ATU(0xE0,"à") ATU(0xE1,"á") ATU(0xE2,"â") ATU(0xE3,"ã") ATU(0xE4,"ä") ATU(0xE5,"å") ATU(0xE6,"æ") ATU(0xE7,"ç")
        ATU(0xE8,"è") ATU(0xE9,"é") ATU(0xEA,"ê") ATU(0xEB,"ë") ATU(0xEC,"ì") ATU(0xED,"í") ATU(0xEE,"î") ATU(0xEF,"ï")
        ATU(0xF0,"ð") ATU(0xF1,"ñ") ATU(0xF2,"ò") ATU(0xF3,"ó") ATU(0xF4,"ô") ATU(0xF5,"õ") ATU(0xF6,"ö") ATU(0xF7,"÷")
        ATU(0xF8,"ø") ATU(0xF9,"ù") ATU(0xFA,"ú") ATU(0xFB,"û") ATU(0xFC,"ü") ATU(0xFD,"ý") ATU(0xFE,"þ") ATU(0xFF,"ÿ")
    }
  }
#undef ATU
  return str;
}
