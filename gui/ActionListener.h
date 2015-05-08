#ifndef DEF_L_GUI_ActionListener
#define DEF_L_GUI_ActionListener

#include <functional>
#include "../dynamic.h"
#include "Layer.h"

namespace L {
  namespace GUI {
    class Event {
      public:
        enum Type {
          leftClick,
          mouseMove,
          mouseOver,
          mouseOut,
          wheel,
          keyDown,
          usedEvent
        } type;
        union {
          struct {int x, y;};
        };
        Event(Type, int x=0, int y=0);
    };
    class ActionListener : public Layer {
      public:
        typedef std::function<bool(ActionListener*,Dynamic::Var&,Event)> Function;
      protected:
        Function _f; // A return value of true means the event has been used
        Dynamic::Var _param;
        bool _mouseIsOver;

      public:
        ActionListener(const Ref<Base>&, const Function&, const Dynamic::Var& param = Dynamic::Var());

        bool event(const Window::Event&);
    };
  }
}

#endif



