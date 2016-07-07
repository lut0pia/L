#ifndef DEF_L_GUI_Text
#define DEF_L_GUI_Text

#include <cstdlib>
#include "Image.h"
#include "../font/FontBase.h"

namespace L {
  namespace GUI {
    class Text : public Image {
      protected:
        String _text;
        Font::Base& _font;
        Color _color;
        void redraw();

      public:
        Text(const String& text = "", const String& font = "", Color = Color::white);
        virtual ~Text() {}
        void text(const String&, Color = Color::white);
        const String& text() const {return _text;}
    };
  }
}

#endif



