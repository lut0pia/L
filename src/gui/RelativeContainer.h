#ifndef DEF_L_GUI_RelativeContainer
#define DEF_L_GUI_RelativeContainer

#include <cstdlib>
#include "Sizable.h"
#include "../containers/Set.h"

namespace L {
  namespace GUI {
    static const Vector2f TL(0.f,0.f),CL(0.f,.5f),BL(0.f,1.f),TC(.5f,0.f),CC(.5f,.5f),BC(.5f,1.f),TR(1.f,0.f),CR(1.f,.5f),BR(1.f,1.f);
    typedef struct {
      Vector2i relPixPos; // Relative position to matched point in pixels
      Vector<2,float> srcRatPos,tarRatPos; // Matching points ratio
    } RelPos;
    class RelativeContainer : public Sizable {
      protected:
        Map<Base*,RelPos> positions;
        Set<Ref<Base> > elements;

      public:
        RelativeContainer();
        RelativeContainer(const Vector<2,int>&);
        virtual ~RelativeContainer() {}

        void place(const Ref<Base>&, const Vector<2,int>& relPixPos = Vector<2,int>(0,0), const Vector<2,float>& srcRatPos = TL, const Vector<2,float>& tarRatPos = TL);
        void detach(Ref<Base>);
        void clear();

        void dimensionsChanged(Base*,Vector<2,int>);
        void updateFromAbove(Vector<2,int>,Interval<2,int>);
        void draw(GL::Program&);
        bool event(const Window::Event&);

    };
  }
}

#endif




