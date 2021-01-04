#pragma once

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
"struct GBufferSample { vec3 color; float metalness; vec3 normal; float roughness; float emissive; vec3 position; float depth; float linear_depth; };" \
"vec2 coords_correction(vec2 _coords) { return _coords; }" \
"vec3 linearize(vec3 c){" \
"return pow(c, vec3(2.2f));" \
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
  "vec4 position_p = invViewProj * vec4(coords_correction(texcoords*2.f-1.f), s.depth, 1.f);" \
  "s.position = position_p.xyz/position_p.w;" \
  "return s;" \
"}"
