#include "TextInput.h"

#include <algorithm>

#include "GUI.h"

using namespace L;
using namespace L::GUI;

void TextInput::updateText() {
  String tmp = text;
  Color c(Color::black);
  if(password) tmp = String(tmp.size(),'*');
  if(focus) tmp.insert(cursor,1);
  if(tmp=="" && !focus) {
    tmp = placeholder;
    c = Color::grey;
  }
  textElement->text(tmp,c);
}
void TextInput::updateTextElement(Vector2i dim) {
  textElement->updateFromAbove(Vector<2,int>(std::min(pos.x(),pos.x()+dimensions.x()-dim.x()),
                               ((2*pos.y()+dimensions.y())/2)-(dim.y()/2)),clip);
}

TextInput::TextInput(Vector2i d, const String& font, String placeholder, bool password) : Sizable(d), textElement(new Text("",font)) {
  textElement->sParent(this);
  this->placeholder = placeholder;
  this->password = password;
  focus = false;
  cursor = 0;
  updateText();
}

String TextInput::gText() const {
  return text;
}

bool TextInput::event(const Window::Event& e) {
  if(focus) {
    if(e.type == Window::Event::TEXT) {
      text.insert(cursor,e.text);
      cursor+=strlen(e.text);
      updateText();
      return true;
    } else if(e.type == Window::Event::BUTTONDOWN) {
      switch(e.button) {
        case Window::Event::BACKSPACE:
          if(cursor) {
            while(((byte)text[cursor-1]>>6)==0x2) {
              text.erase(cursor-1,1);
              cursor--;
            }
            text.erase(cursor-1,1);
            cursor--;
          }
          break;
        case Window::Event::ENTER:
          text.insert(cursor,'\n');
          cursor++;
          break;
        case Window::Event::LEFT:
          if(cursor) {
            cursor--;
            while(((byte)text[cursor]>>6)==0x2) {
              cursor--;
            }
          }
          break;
        case Window::Event::RIGHT:
          if(cursor<text.size()) {
            cursor++;
            while(((byte)text[cursor]>>6)==0x2) {
              cursor++;
            }
          }
          break;
        default:
          return false;
      }
      updateText();
      return true;
    }
  }
  if(e.type == Window::Event::BUTTONDOWN && e.button == Window::Event::LBUTTON) {
    if(clip.contains(Vector2i(e.x,e.y))) {
      if(!focus) {
        focus = true;
        updateText();
        return true;
      }
    } else if(focus) {
      focus = false;
      updateText();
      return false;
    }
  }
  return false;
}
void TextInput::draw(GL::Program& program) {
  textElement->draw(program);
}
void TextInput::dimensionsChanged(Base* e,Vector2i newDim) {
  updateTextElement(newDim);
}
void TextInput::updateFromAbove(Vector2i pos, Interval2i parentClip) {
  Solid::updateFromAbove(pos,parentClip);
  updateTextElement(textElement->gDimensions());
}

