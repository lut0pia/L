#pragma once 

#define L_SHAREDUNIFORM_SIZE (sizeof(Matrix44f)*5+sizeof(Vector4f)*256+sizeof(Vector4f)*3)
#define L_SHAREDUNIFORM_VIEW 0
#define L_SHAREDUNIFORM_INVVIEW (L_SHAREDUNIFORM_VIEW+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_VIEWPROJ (L_SHAREDUNIFORM_INVVIEW+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_INVVIEWPROJ (L_SHAREDUNIFORM_VIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_PREVVIEWPROJ (L_SHAREDUNIFORM_INVVIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_DITHERMATRIX (L_SHAREDUNIFORM_PREVVIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_DITHERMATRIXSIZE (L_SHAREDUNIFORM_DITHERMATRIX+sizeof(Vector4f)*256)
#define L_SHAREDUNIFORM_EYE (L_SHAREDUNIFORM_DITHERMATRIXSIZE+sizeof(Vector4i))
#define L_SHAREDUNIFORM_FRAME (L_SHAREDUNIFORM_EYE+sizeof(Vector4f))
#define L_SHAREDUNIFORM \
"layout (std140) uniform Shared {" \
"mat4 view, invView, viewProj, invViewProj, prevViewProj;" \
"vec4 ditherMatrix[256];" \
"ivec4 ditherMatrixSize;" \
"vec4 eye;" \
"int frame;" \
"};" \
"bool alpha(float a){" \
"ivec2 frameOffset = ivec2(frame*31,frame*37);" \
"ivec2 ditherPos = ivec2(mod(gl_FragCoord.xy+frameOffset,ditherMatrixSize.xy));" \
"int ditherSlot = ditherPos.x+ditherPos.y*ditherMatrixSize.x;" \
"float ditherThreshold = ditherMatrix[ditherSlot/4][ditherSlot%4];" \
"return a<ditherThreshold;" \
"}" \
"vec2 encodeNormal(vec3 n){" \
"n = normalize((view*vec4(n,0.f)).xyz);" \
"return (n.xy/sqrt(n.z*8.f+8.f))+.5f;" \
"}" \
"vec3 decodeNormal(vec2 e){" \
"e = e*4.f-2.f;" \
"float l = dot(e,e);" \
"float g = sqrt(1.f-l/4.f);" \
"return normalize((invView*vec4(e*g,1.f-l/2.f,0.f)).xyz);" \
"}"