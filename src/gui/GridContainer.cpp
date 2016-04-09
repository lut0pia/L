#include "GridContainer.h"

using namespace L;
using namespace L::GUI;
/*
GridContainer::GridContainer() : Base(), spacing(0,0){}
void GUI_VerticalList::update(){
    width = height = 0;
    L_Iter(elements,it){
        height += (*it)->gHeight();
        if(it!=--elements.end())
            height += space;
        width = max(width,(*it)->gWidth());
    }
    GUI::update();
}

Vector2i GUI_VerticalList::absPos(GUI* asker){
    Vector2i tmp = GUI::absPos(this), modifier(0,0);
    list<Ref<GUI> >::iterator it;
    for(it=elements.begin();it!=elements.end()&&(*it)!=asker;it++){
        modifier.y += (*it)->gHeight();
        if(it!=--elements.end())
            modifier.y += space;
    }
    return (it!=elements.end()) ? tmp+modifier : tmp;
}
*/
