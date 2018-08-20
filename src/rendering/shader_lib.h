#pragma once

#define L_GLSL_INTRO "#version 450\n#extension GL_ARB_separate_shader_objects : require\n"
#define L_SHAREDUNIFORM_VIEW 0
#define L_SHAREDUNIFORM_INVVIEW (L_SHAREDUNIFORM_VIEW+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_PROJECTION (L_SHAREDUNIFORM_INVVIEW+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_VIEWPROJ (L_SHAREDUNIFORM_PROJECTION+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_INVVIEWPROJ (L_SHAREDUNIFORM_VIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_PREVVIEWPROJ (L_SHAREDUNIFORM_INVVIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_EYE (L_SHAREDUNIFORM_PREVVIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_SCREEN (L_SHAREDUNIFORM_EYE+sizeof(Vector4f))
#define L_SHAREDUNIFORM_VIEWPORT (L_SHAREDUNIFORM_SCREEN+sizeof(Vector4f))
#define L_SHAREDUNIFORM_VIEWPORT_PIXEL_SIZE (L_SHAREDUNIFORM_VIEWPORT+sizeof(Vector4f))
#define L_SHAREDUNIFORM_FRAME (L_SHAREDUNIFORM_VIEWPORT_PIXEL_SIZE+sizeof(Vector4f))
#define L_SHAREDUNIFORM_SIZE (L_SHAREDUNIFORM_FRAME+sizeof(int))
#define L_SHAREDUNIFORM \
"layout(binding = 0) uniform Shared {" \
"mat4 view, invView, projection, viewProj, invViewProj, prevViewProj;" \
"vec4 eye, screen, viewport, viewport_pixel_size;" \
"int frame;" \
"};"
#define L_PUSH_CONSTANTS \
"layout(push_constant) uniform Constants {" \
"mat4 model;" \
"};"
#define L_SHADER_LIB \
"const float PI = 3.14159265359f;" \
"const float PHI = 1.61803398874989484820459;" \
"const float SQ2 = 1.41421356237309504880169;" \
"struct GBufferSample { vec3 color; float metalness; vec3 normal; float roughness; float emissive; vec3 position; float depth; float linear_depth; };" \
"float gold_noise(in vec2 coordinate, in float seed) {" \
  "float noise = fract(tan(distance(coordinate*(seed+PHI*00000.1), vec2(PHI*00000.1, PI*00000.1)))*SQ2*10000.0);" \
  "return isnan(noise) ? 0.f : noise;" \
"}" \
"float frag_noise() {" \
  "return gold_noise(gl_FragCoord.xy, frame%4);" \
"}" \
"bool alpha(float a){" \
"return a<clamp(frag_noise(), 0.01f, 0.99f);" \
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
"float linearize_depth(float depth) {" \
  "return projection[3][2] / (projection[2][2] + (2.f * depth - 1.f));" \
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
"}" \
"GBufferSample sample_gbuffer(sampler2D color_buffer, sampler2D normal_buffer, sampler2D depth_buffer) {" \
  "GBufferSample s;" \
  "vec2 texcoords = gl_FragCoord.xy*viewport_pixel_size.zw;" \
  "vec4 color_sample = texture(color_buffer, texcoords);" \
  "s.color = color_sample.rgb;" \
  "s.metalness = color_sample.a;" \
  "vec4 normal_sample = texture(normal_buffer, texcoords);" \
  "s.normal = decodeNormal(normal_sample.xy);" \
  "s.roughness = normal_sample.z;" \
  "s.emissive = normal_sample.w;" \
  "s.depth = texture(depth_buffer, texcoords).r;" \
  "s.linear_depth = linearize_depth(s.depth);" \
  "vec4 position_p = invViewProj * vec4(texcoords*2.f-1.f, s.depth, 1.f);" \
  "s.position = position_p.xyz/position_p.w;" \
  "return s;" \
"}"
