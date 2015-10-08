#ifndef DEF_L_GUI_RelativeContainer
#define DEF_L_GUI_RelativeContainer

#include <cstdlib>
#include "Sizable.h"
#include "../containers/Set.h"

namespace L {
  namespace GUI {
    static const Point<2,float> TL(0,0),CL(0,.5),BL(0,1),TC(.5,0),CC(.5,.5),BC(.5,1),TR(1,0),CR(1,.5),BR(1,1);
    typedef struct {
      Point2i relPixPos; // Relative position to matched point in pixels
      Point<2,float> srcRatPos,tarRatPos; // Matching points ratio
    } RelPos;
    class RelativeContainer : public Sizable {
      protected:
        Map<Base*,RelPos> positions;
        Set<Ref<Base> > elements;

      public:
        RelativeContainer();
        RelativeContainer(const Point<2,int>&);
        virtual ~RelativeContainer() {}

        void place(const Ref<Base>&, const Point<2,int>& relPixPos = Point<2,int>(0,0), const Point<2,float>& srcRatPos = TL, const Point<2,float>& tarRatPos = TL);
        void detach(Ref<Base>);
        void clear();

        void dimensionsChanged(Base*,Point<2,int>);
        void updateFromAbove(Point<2,int>,Interval<2,int>);
        void draw(GL::Program&);
        bool event(const Window::Event&);

    };
  }
}

#endif




