#include "encoding.h"

using namespace L;

uint32_t L::utf8_to_utf32(const char*& str) {
  uint32_t wtr((uint8_t)*str);
  if(wtr>>5==0x6) { // 2 bytes
    wtr <<= 6;
    wtr |= ((uint8_t)*(++str)) & bitmask(6);
    wtr &= bitmask(11);
  } else if(wtr>>4==0xE) { // 3 bytes
    wtr <<= 6;
    wtr |= ((uint8_t)*(++str)) & bitmask(6);
    wtr <<= 6;
    wtr |= ((uint8_t)*(++str)) & bitmask(6);
    wtr &= bitmask(16);
  }
  str++;
  return wtr;
}
uint32_t L::utf8_to_utf32(const char* str, int* size) {
  uint32_t wtr((uint8_t)*str);
  if(wtr>>5==0x6) { // 2 bytes
    wtr <<= 6;
    wtr |= ((uint8_t)*(++str)) & bitmask(6);
    wtr &= bitmask(11);
    *size = 2;
  } else if(wtr>>4==0xE) { // 3 bytes
    wtr <<= 6;
    wtr |= ((uint8_t)*(++str)) & bitmask(6);
    wtr <<= 6;
    wtr |= ((uint8_t)*(++str)) & bitmask(6);
    wtr &= bitmask(16);
    *size = 3;
  } else *size = 1;
  return wtr;
}
Array<uint32_t> L::utf8_to_utf32_array(const char* str) {
  Array<uint32_t> wtr;
  while(str) wtr.push(utf8_to_utf32(str));
  return wtr;
}
String L::ansi_to_utf8(String str) {
#define ATU(a,u) case a: \
    str.replace(i,1,u); \
    i += String(u).size()-1; \
    break;
  for(size_t i(0); i<str.size(); i++) {
    switch((uint8_t)str[i]) {
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
const char* L::utf16_to_utf8(uint16_t utf16) {
  thread_local char wtr[4];
  if(utf16<(1<<7)) {
    wtr[0] = (char)utf16;
    wtr[1] = 0;
  } else if(utf16<(1<<11)) {
    wtr[0] = 0xC0i8 | char(utf16>>6);
    wtr[1] = 0x80i8 | char(utf16 & bitmask(6));
    wtr[2] = 0;
  } else {
    wtr[0] = 0xE0i8 | char(utf16>>12);
    wtr[1] = 0x80i8 | char((utf16>>6) & bitmask(6));
    wtr[2] = 0x80i8 | char(utf16 & bitmask(6));
    wtr[3] = 0;
  }
  return wtr;
}
const char* L::utf32_to_utf8(uint32_t utf32) {
  thread_local char wtr[5];
  if(utf32<(1<<7)) {
    wtr[0] = char(utf32);
    wtr[1] = 0;
  } else if(utf32<(1<<11)) {
    wtr[0] = 0xC0i8 | char(utf32>>6);
    wtr[1] = 0x80i8 | char(utf32 & bitmask(6));
    wtr[2] = 0;
  } else if(utf32<(1<<11)) {
    wtr[0] = 0xE0i8 | char(utf32>>12);
    wtr[1] = 0x80i8 | char((utf32>>6) & bitmask(6));
    wtr[2] = 0x80i8 | char(utf32 & bitmask(6));
    wtr[3] = 0;
  } else {
    wtr[0] = 0xF0i8 | char(utf32>>18);
    wtr[1] = 0x80i8 | char((utf32>>12) & bitmask(6));
    wtr[2] = 0x80i8 | char((utf32>>6) & bitmask(6));
    wtr[3] = 0x80i8 | char(utf32 & bitmask(6));
    wtr[4] = 0;
  }
  return wtr;
}

String L::url_encode(const String& src) {
  String wtr;
  for(char c : src){
    if(strchr("%!#$&'()*+,/:;=?@[]", c)) {
      wtr.push('%');
      wtr += ntos<16>(c, 2);
    } else wtr.push(c);
  }
  return wtr;
}
String L::url_decode(const String& src) {
  String wtr;
  int state(0);
  char buffer[3]{};
  for(char c : src) {
    switch(state) {
      case 1:
        buffer[0] = c;
        state = 2;
        break;
      case 2:
        buffer[1] = c;
        wtr.push(char(ston<16, uint32_t>(buffer)));
        state = 0;
        break;
      default:
        if(c=='%') state = 1;
        else wtr.push(c);
        break;
    }
  }
  return wtr;
}
