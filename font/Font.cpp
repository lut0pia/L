#include "Font.h"

#include "../containers/Ref.h"

using namespace L;

Map<String,Ref<Font::Base> > fonts;

Font::Base& Font::get(const String& name){
    if(fonts.has(name))
        return *fonts[name];
    else throw Exception("Font: No font named "+name+".");
}
void Font::set(Base* font, const String& name){
    fonts[name] = font;
}
