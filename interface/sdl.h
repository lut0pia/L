#ifndef DEF_Li_sdl
#define DEF_Li_sdl

#include <L/L.h>
#include <SDL/SDL.h>

L::System::Window::Event eventFromSDL(SDL_Event* event){
    L::System::Window::Event wtr;
    switch(event->type){
        case SDL_MOUSEMOTION:
            wtr.type = L::System::Window::Event::MOUSEMOVE;
            wtr.x = event->motion.x;
            wtr.y = event->motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            wtr.type = L::System::Window::Event::LBUTTONDOWN;
            wtr.x = event->motion.x;
            wtr.y = event->motion.y;
            break;
        case SDL_TEXTINPUT:
            wtr.type = L::System::Window::Event::TEXT;
            strcpy(wtr.text,event->text.text);
            break;
        case SDL_KEYDOWN:
            wtr.type = L::System::Window::Event::KEYDOWN;
            switch(event->key.keysym.sym){
                case '\b': wtr.key = L::System::Window::Event::BACKSPACE; break;
                case '\r': wtr.key = L::System::Window::Event::ENTER; break;
                case SDLK_LEFT: wtr.key = L::System::Window::Event::LEFT; break;
                case SDLK_RIGHT: wtr.key = L::System::Window::Event::RIGHT; break;
            }
            break;
        case SDL_MOUSEWHEEL:
            wtr.type = L::System::Window::Event::MOUSEWHEEL;
            wtr.x = event->wheel.x;
            wtr.y = event->wheel.y;
            break;
        default: break;
    }
    return wtr;
}
/*
bool isMouseMotion(void* event){
    return (((SDL_Event*)event)->type == SDL_MOUSEMOTION);
}
bool isLeftClick(void* event){
    return (((SDL_Event*)event)->type == SDL_MOUSEBUTTONDOWN);
}
bool isLeftClickUp(void* event){
    return (((SDL_Event*)event)->type == SDL_MOUSEBUTTONUP);
}
P2D<int> mouseWheel(void* event){
    P2D<int> wtr(0,0);
    if(((SDL_Event*)event)->type == SDL_MOUSEWHEEL){
        wtr.x -= ((SDL_Event*)event)->wheel.x;
        wtr.y -= ((SDL_Event*)event)->wheel.y;
    }
    return wtr;
}
bool isKeyDown(void* event){
    return (((SDL_Event*)event)->type == SDL_KEYDOWN);
}
P2D<int> mousePos(void* event){
    return P2D<int>(((SDL_Event*)event)->motion.x,((SDL_Event*)event)->motion.y);
}

void startTextInput(){
    SDL_StartTextInput();
}
void stopTextInput(){
    SDL_StopTextInput();
}
bool textInput(void* event, string& text, size_t& cursor){
    if(((SDL_Event*)event)->type == SDL_TEXTINPUT){
        text.insert(cursor,((SDL_Event*)event)->text.text);
        cursor+=strlen(((SDL_Event*)event)->text.text);
        return true;
    }
    else if(((SDL_Event*)event)->type == SDL_KEYDOWN){
        int key = ((SDL_Event*)event)->key.keysym.sym;
        if(key == '\b'){ // Backspace
            if(cursor){
                while(((byte)text[cursor-1]>>6)==0x2){
                    text.erase(cursor-1,1);
                    cursor--;
                }
                text.erase(cursor-1,1);
                cursor--;
                return true;
            }
        }
        else if(key=='\r'){ // Enter
            text.insert(cursor,1,'\n');
            cursor++;
            return true;
        }
        else if(key==SDLK_LEFT){
            if(cursor){
                cursor--;
                while(((byte)text[cursor]>>6)==0x2){
                    cursor--;
                }
                return true;
            }
        }
        else if(key==SDLK_RIGHT){
            if(cursor<text.size()){
                cursor++;
                while(((byte)text[cursor]>>6)==0x2){
                    cursor++;
                }
                return true;
            }
        }
    }
    return false;
}
*/

#endif

