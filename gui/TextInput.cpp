#include "TextInput.h"

#include "GUI.h"

using namespace L;
using namespace L::GUI;

void TextInput::updateText(){
    String tmp = text;
    Color c(Color::black);
    if(password) tmp = String(tmp.size(),'*');
    if(focus) tmp.insert(cursor,1,1);

    if(tmp=="" && !focus){
        tmp = placeholder;
        c = Color::grey;
    }
    textElement->sText(tmp,c);
}
void TextInput::updateTextElement(Point2i dim){
    textElement->updateFromAbove(Point<2,int>(std::min(pos.x(),pos.x()+dimensions.x()-dim.x()),
                                              ((2*pos.y()+dimensions.y())/2)-(dim.y()/2)),clip);
}

TextInput::TextInput(Point2i d, const String& font, String placeholder, bool password) : Sizable(d), textElement(new Text("",font)){
    textElement->sParent(this);
    this->placeholder = placeholder;
    this->password = password;
    focus = false;
    cursor = 0;
    updateText();
}

String TextInput::gText() const{
    return text;
}

bool TextInput::event(const Window::Event& e){
    if(focus){
        if(e.type == Window::Event::TEXT){
            text.insert(cursor,e.text);
            cursor+=strlen(e.text);
            updateText();
            return true;
        }
        else if(e.type == Window::Event::KEYDOWN){
            switch(e.key){
                case Window::Event::BACKSPACE:
                    if(cursor){
                        while(((byte)text[cursor-1]>>6)==0x2){
                            text.erase(cursor-1,1);
                            cursor--;
                        }
                        text.erase(cursor-1,1);
                        cursor--;
                    }
                    break;
                case Window::Event::ENTER:
                    text.insert(cursor,1,'\n');
                    cursor++;
                    break;
                case Window::Event::LEFT:
                    if(cursor){
                        cursor--;
                        while(((byte)text[cursor]>>6)==0x2){
                            cursor--;
                        }
                    }
                    break;
                case Window::Event::RIGHT:
                    if(cursor<text.size()){
                        cursor++;
                        while(((byte)text[cursor]>>6)==0x2){
                            cursor++;
                        }
                    }
                    break;
                default: return false;
            }
            updateText();
            return true;
        }
    }
    if(e.type == Window::Event::LBUTTONDOWN){
        if(clip.contains(Point<2,int>(e.x,e.y))){
            if(!focus){
                focus = true;
                updateText();
                return true;
            }
        }
        else if(focus){
            focus = false;
            updateText();
            return false;
        }
    }
    return false;
}
void TextInput::draw(){
    textElement->draw();
}
void TextInput::dimensionsChanged(Base* e,Point2i newDim){
    updateTextElement(newDim);
}
void TextInput::updateFromAbove(Point2i pos, Interval2i parentClip){
    Solid::updateFromAbove(pos,parentClip);
    updateTextElement(textElement->gDimensions());
}

