#include "Color.h"

#include <cstring>
#include "../bytes.h"
#include "../Exception.h"
#include "../Rand.h"

using namespace L;

const Color Color::black(0,0,0);
const Color Color::blue(0,0,255);
const Color Color::green(0,255,0);
const Color Color::grey(128,128,128);
const Color Color::lightGrey(192,192,192);
const Color Color::red(255,0,0);
const Color Color::white(255,255,255);
const Color Color::yellow(255,255,0);

Color::Color(){
    data[0] =
    data[1] =
    data[2] =
    data[3] = 0;
}
Color::Color(byte gs){
    data[0] =
    data[1] =
    data[2] = gs;
    data[3] = 0xFF;
}
Color::Color(byte r, byte g, byte b, byte a){
    data[0] = r;
    data[1] = g;
    data[2] = b;
    data[3] = a;
}

bool Color::operator==(const Color& other) const{
    return !memcmp(data,other.data,4);
}
bool Color::operator!=(const Color& other) const{
    return !(*this == other);
}
bool Color::operator<(const Color& other) const{
    for(uint i(0);i<4;i++)
        if(data[i]<other.data[i])
            return true;
        else if(data[i]>other.data[i])
            return false;
    return false;
}
Color& Color::operator+=(const Color& other){
    for(size_t i=0;i<4;i++){
        if((data[i]+other.data[i])%0xFF < data[i])
            data[i] = 0xFF;
        else
            data[i] += other.data[i];
    }
    return *this;
}

byte Color::r() const{return data[0];}
byte Color::g() const{return data[1];}
byte Color::b() const{return data[2];}
byte Color::a() const{return data[3];}
byte& Color::r(){return data[0];}
byte& Color::g(){return data[1];}
byte& Color::b(){return data[2];}
byte& Color::a(){return data[3];}

void Color::write(std::ostream& os) const{
    os.write((char*)data,4);
}
void Color::read(std::istream& is){
    is.read((char*)data,4);
}

Color Color::from(const String& str){
    if(str.size()==7 && str[0]=='#'){ // Hexa color
        Vector<byte> tmp(htb(str.substr(1)));
        return Color(tmp[0],tmp[1],tmp[2]);
    }
    #define TMP(name,color) else if(str.toLower()==name) return color;
    TMP("black",black)
    TMP("blue",blue)
    TMP("green",green)
    TMP("grey",grey)
    TMP("lightgrey",lightGrey)
    TMP("red",red)
    TMP("white",white)
    #undef TMP
    else throw Exception("Unknown color "+str);
}
Color Color::from(float r, float g, float b, float a){
    return Color(r*255,g*255,b*255,a*255);
}
Color Color::random(){
    return Color(Rand::nextByte(),Rand::nextByte(),Rand::nextByte());
}


