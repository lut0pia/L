#ifndef DEF_L_GUI_Text
#define DEF_L_GUI_Text

#include <cstdlib>
#include "Image.h"
#include "../font.h"

namespace L{
    namespace GUI{
        class Text : public Image{
            protected:
                String text;
                Font::Base& font;
                Color color;
                void redraw();

            public:
                Text(const String& text = "", const String& font = "", Color = Color::white);
                virtual ~Text(){}

                // Setters
                void sText(const String&, Color = Color::white);

                // Getters
                String gText() const{return text;}
        };
    }
}

#endif



