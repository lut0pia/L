#pragma once

#define L_SHAREDUNIFORM_VIEW 0
#define L_SHAREDUNIFORM_INVVIEW (L_SHAREDUNIFORM_VIEW+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_VIEWPROJ (L_SHAREDUNIFORM_INVVIEW+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_INVVIEWPROJ (L_SHAREDUNIFORM_VIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_PREVVIEWPROJ (L_SHAREDUNIFORM_INVVIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_DITHERMATRIX (L_SHAREDUNIFORM_PREVVIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_DITHERMATRIXSIZE (L_SHAREDUNIFORM_DITHERMATRIX+sizeof(Vector4f)*256)
#define L_SHAREDUNIFORM_EYE (L_SHAREDUNIFORM_DITHERMATRIXSIZE+sizeof(Vector4i))
#define L_SHAREDUNIFORM_SCREEN (L_SHAREDUNIFORM_EYE+sizeof(Vector4f))
#define L_SHAREDUNIFORM_VIEWPORT (L_SHAREDUNIFORM_SCREEN+sizeof(Vector4f))
#define L_SHAREDUNIFORM_FRAME (L_SHAREDUNIFORM_VIEWPORT+sizeof(Vector4f))
#define L_SHAREDUNIFORM_SIZE (L_SHAREDUNIFORM_FRAME+sizeof(int))
#define L_SHAREDUNIFORM \
"layout (std140) uniform Shared {" \
"mat4 view, invView, viewProj, invViewProj, prevViewProj;" \
"vec4 ditherMatrix[256];" \
"ivec4 ditherMatrixSize;" \
"vec4 eye;" \
"vec4 screen, viewport;" \
"int frame;" \
"};"
#define L_SHADER_LIB \
"const float PI = 3.14159265359f;" \
"float frag_noise(){" \
"int sub_frame = frame%4;" \
"ivec2 frame_offset = ivec2(sub_frame/2,sub_frame%2);" \
"ivec2 dither_pos = ivec2(mod(gl_FragCoord.xy+frame_offset,ditherMatrixSize.xy));" \
"int dither_slot = dither_pos.x+dither_pos.y*ditherMatrixSize.x;" \
"return ditherMatrix[dither_slot/4][dither_slot%4];" \
"}" \
"bool alpha(float a){" \
"return a<frag_noise();" \
"}" \
"vec3 deband(vec3 v){" \
"return v+frag_noise()*0.005f;" \
"}" \
"vec3 derive_normal(vec3 p, vec3 n, float h){" \
"vec3 dpdx = dFdx(p);" \
"vec3 dpdy = dFdy(p);" \
"float dhdx = dFdx(h);" \
"float dhdy = dFdy(h);" \
"vec3 r1 = cross(dpdy, n);" \
"vec3 r2 = cross(n, dpdx);" \
"vec3 g = (r1 * dhdx + r2 * dhdy) / dot(dpdx, r1);" \
"return normalize(n - g);" \
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
