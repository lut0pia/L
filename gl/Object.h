#ifndef DEF_L_Object
#define DEF_L_Object

#include <GL/gl.h>

#include "../containers/Ref.h"
#include "../gl/Texture.h"
#include "PRS.h"
#include "Mesh.h"

namespace L{
    class Object{
        public:     Ref<PRS> prs;
        private:    Ref<Mesh> mesh;
                    Ref<GL::Texture> texture;

        public:
            Object(const Ref<PRS>&, const Ref<Mesh>&, const Ref<GL::Texture>& texture);
            void draw();
    };
}

#endif




