#pragma once

#include <functional>
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
        typedef std::function<bool(ActionListener*,Var&,Event)> Function;
      protected:
        Function _f; // A return value of true means the event has been used
        Var _param;
        bool _mouseIsOver;

      public:
        ActionListener(const Ref<Base>&, const Function&, const Var& param = Var());

        bool event(const Window::Event&);
    };
  }
}
