#pragma once 

#define L_SHAREDUNIFORM_SIZE (sizeof(Matrix44f)*64+sizeof(Matrix44f)*2+sizeof(Vector4i)+sizeof(Vector3f))
#define L_SHAREDUNIFORM_DITHERMATRIX 0
#define L_SHAREDUNIFORM_VIEWPROJ (L_SHAREDUNIFORM_DITHERMATRIX+sizeof(Matrix44f)*64)
#define L_SHAREDUNIFORM_INVVIEWPROJ (L_SHAREDUNIFORM_VIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_DITHERMATRIXSIZE (L_SHAREDUNIFORM_INVVIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_EYE (L_SHAREDUNIFORM_DITHERMATRIXSIZE+sizeof(Vector4i))
#define L_SHAREDUNIFORM \
"layout (std140) uniform Shared {" \
"mat4 ditherMatrix[64];" \
"mat4 viewProj, invViewProj;" \
"ivec4 ditherMatrixSize;" \
"vec3 eye;" \
"};" \
"bool alpha(float a){" \
"ivec2 ditherPos = ivec2(mod(gl_FragCoord.xy,ditherMatrixSize.xy));" \
"int ditherSlot = ditherPos.x+ditherPos.y*ditherMatrixSize.x;" \
"float ditherThreshold = ditherMatrix[ditherSlot/16][(ditherSlot%16)/4][ditherSlot%4];" \
"return a<ditherThreshold;" \
"}"