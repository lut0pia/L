#ifndef DEF_L_GL_Camera
#define DEF_L_GL_Camera

#include "PRS.h"
#include "../containers/Ref.h"

namespace L {
  namespace GL {
    class Camera {
      public:
        Ref<PRS> prs;
      protected:
        double fovy, aspect, nearLimit, farLimit;

      public:
        Camera(const Ref<PRS>&, double fovy = 70, double aspect = 16.0/9.0, double nearLimit = .01, double farLimit = 512);

        void sPRS(const Ref<PRS>&);
        void sFovy(double fovy);
        void sAspect(double aspect);
        void sClipping(double near, double far);

        void aPerspective();
        void place();

    };
  }
}

#endif





