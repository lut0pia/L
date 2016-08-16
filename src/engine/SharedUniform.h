#pragma once 

#define L_SHAREDUNIFORM_SIZE (sizeof(Matrix44f)*4+sizeof(Vector4f)*256+sizeof(Vector4i)+sizeof(Vector3f))
#define L_SHAREDUNIFORM_VIEW 0
#define L_SHAREDUNIFORM_INVVIEW (L_SHAREDUNIFORM_VIEW+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_VIEWPROJ (L_SHAREDUNIFORM_INVVIEW+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_INVVIEWPROJ (L_SHAREDUNIFORM_VIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_DITHERMATRIX (L_SHAREDUNIFORM_INVVIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_DITHERMATRIXSIZE (L_SHAREDUNIFORM_DITHERMATRIX+sizeof(Vector4f)*256)
#define L_SHAREDUNIFORM_EYE (L_SHAREDUNIFORM_DITHERMATRIXSIZE+sizeof(Vector4i))
#define L_SHAREDUNIFORM \
"layout (std140) uniform Shared {" \
"mat4 view, invView, viewProj, invViewProj;" \
"vec4 ditherMatrix[256];" \
"ivec4 ditherMatrixSize;" \
"vec3 eye;" \
"};" \
"bool alpha(float a){" \
"ivec2 ditherPos = ivec2(mod(gl_FragCoord.xy,ditherMatrixSize.xy));" \
"int ditherSlot = ditherPos.x+ditherPos.y*ditherMatrixSize.x;" \
"float ditherThreshold = ditherMatrix[ditherSlot/4][ditherSlot%4];" \
"return a<ditherThreshold;" \
"}"