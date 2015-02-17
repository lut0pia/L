#include "Object.h"

using namespace L;

Object::Object(const Ref<PRS>& prs, const Ref<Mesh>& mesh, const Ref<GL::Texture>& texture)
: prs(prs), mesh(mesh), texture(texture){}

void Object::draw(){
    glPushMatrix();
        glTranslated(prs->position.x(),prs->position.y(),prs->position.z());
        glRotated(prs->rotation.z(),0,0,1);
        glRotated(prs->rotation.y(),0,1,0);
        glRotated(prs->rotation.x(),1,0,0);
        glScaled(prs->scale.x(),prs->scale.y(),prs->scale.z());

        glBindTexture(GL_TEXTURE_2D, texture->id());
        mesh->draw();
        glBindTexture(GL_TEXTURE_2D,0);
    glPopMatrix();
}
/*

#include <iostream>
#include <cmath>

#include "PRS.h"

L_Object::L_Object(){
    prs = new L_PRS;
    prs->reset();
    mesh = NULL;
    textureId = 0;
}
void L_Object::load(String meshFilePath, String texFilePath){
    mesh = L_gMesh(meshFilePath);
    textureId = L_gTexture(texFilePath);
}
void L_Object::draw(){
    glPushMatrix();
        glTranslated(prs->position.x,prs->position.y,prs->position.z);
        glRotatef(prs->rotation.z,0,0,1);
        glRotatef(prs->rotation.y,0,1,0);
        glRotatef(prs->rotation.x,1,0,0);
        glScalef(prs->scale.x,prs->scale.y,prs->scale.z);

        glBindTexture(GL_TEXTURE_2D, textureId);
        mesh->draw();
    glPopMatrix();
}

*/
