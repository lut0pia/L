#pragma once

#include <cstdlib>
#include "Text.h"
#include "Sizable.h"

namespace L {
  namespace GUI {
    class TextInput : public Sizable {
      protected:
        size_t cursor;
        bool focus, password;
        Ref<Text> textElement;
        String text, placeholder;

        void updateText();
        void updateTextElement(Vector<2,int>);

      public:
        TextInput(Vector<2,int>, const String& font = "", String placeholder = "", bool password = false);
        virtual ~TextInput() {}

        String gText() const;

        bool event(const Window::Event& e);
        void draw(GL::Program&);
        void dimensionsChanged(Base*,Vector<2,int>);
        void updateFromAbove(Vector<2,int>,Interval<2,int>);
    };
  }
}
