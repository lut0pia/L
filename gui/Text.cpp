#include "Text.h"

using namespace L;
using namespace GUI;

void GUI::Text::redraw(){
    bitmap(font.render(text).filter(color));
}
GUI::Text::Text(const String& text, const String& font, Color color)
    : Image(), text(text), font(Font::get(font)), color(color){
    redraw();
}
void GUI::Text::sText(const String& text, Color color){
    this->text = text;
    this->color = color;
    redraw();
}


