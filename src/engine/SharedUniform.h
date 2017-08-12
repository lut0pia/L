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
"ivec2 frame_offset = ivec2(sub_frame/2,sub_frame%2)*2;" \
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
"vec3 linearize(vec3 c){" \
"return pow(c, vec3(2.2f));" \
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
"}" \
"float light_attenuation(float dist, float radius, float intensity){" \
  "float num = clamp(1.f-pow(dist/radius,4.f),0.f,1.f);" \
  "return intensity*num*num/(dist*dist+1.f);" \
"}" \
"vec3 fresnel_schlick(float cos_theta, vec3 F0){" \
  "return F0 + (1.0 - F0) * pow(1.0 - cos_theta, 5.0);" \
"}" \
"float distribution_GGX(vec3 N, vec3 H, float roughness){" \
  "float a = roughness*roughness;" \
  "float a2 = a*a;" \
  "float NdotH = max(dot(N, H), 0.0);" \
  "float NdotH2 = NdotH*NdotH;" \
  "float nom = a2;" \
  "float denom = (NdotH2 * (a2 - 1.0) + 1.0);" \
  "denom = PI * denom * denom;" \
  "return nom / denom;" \
"}" \
"float geometry_schlick_GGX(float NdotV, float roughness){" \
  "float r = (roughness + 1.0);" \
  "float k = (r*r) / 8.0;" \
  "float nom = NdotV;" \
  "float denom = NdotV * (1.0 - k) + k;" \
  "return nom / denom;" \
"}" \
"float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness){" \
  "float NdotV = max(dot(N, V), 0.0);" \
  "float NdotL = max(dot(N, L), 0.0);" \
  "float ggx2 = geometry_schlick_GGX(NdotV, roughness);" \
  "float ggx1 = geometry_schlick_GGX(NdotL, roughness);" \
  "return ggx1 * ggx2;" \
"}" \
"vec3 specular(float NDF, float G, vec3 F, vec3 N, vec3 V, vec3 L){" \
  "vec3 nominator = NDF * G * F;" \
  "float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001f;" \
  "return nominator / denominator;" \
"}"
