#ifndef DEF_L_GUI_ActionListener
#define DEF_L_GUI_ActionListener

#include <cstdlib>
#include "../dynamic.h"
#include "Layer.h"

namespace L{
    namespace GUI{
        class Event{
            public:
                enum Type{
                    leftClick,
                    mouseMove,
                    mouseOver,
                    mouseOut,
                    wheel,
                    keyDown,
                    usedEvent
                } type;
                union{
                    struct{int x, y;};
                };
                Event(Type, int x=0, int y=0);
        };
        class ActionListener : public Layer{
            public:
                typedef bool (*Function)(ActionListener*,Dynamic::Var&,Event);
            protected:
                Function f; // A return value of true means the event has been used
                Dynamic::Var param;
                bool mouseIsOver;

            public:
                ActionListener(const Ref<Base>&, Function, Dynamic::Var param = Dynamic::Var());

                bool event(const Window::Event&);
        };
    }
}

#endif



