#include "Camera.h"

#include <GL/gl.h>
#include <GL/glu.h>

using namespace L;

Camera::Camera(const Ref<PRS>& prs, double fovy, double aspect, double nearLimit, double farLimit)
: prs(prs), fovy(fovy), aspect(aspect), nearLimit(nearLimit), farLimit(farLimit){
    aPerspective();
}

void Camera::sFovy(double f){
    fovy = f;
    aPerspective();
}
void Camera::sAspect(double a){
    aspect = a;
    aPerspective();
}
void Camera::sClipping(double n, double f){
    nearLimit = n;
    farLimit = f;
    aPerspective();
}

void Camera::aPerspective(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy,aspect,nearLimit,farLimit);
}
void Camera::place(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotated(-prs->rotation.x()-90,1,0,0);
    glRotated(-prs->rotation.y(),0,1,0);
    glRotated(-prs->rotation.z(),0,0,1);
    glTranslated(-prs->position.x(),-prs->position.y(),-prs->position.z());
}

