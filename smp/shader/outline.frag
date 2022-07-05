#include <shader/alpha_lib.glsl>

layout(location = 0) out vec4 fragcolor;
layout(binding = 1) uniform Parameters {
  float thickness;
};

layout(binding = 2) uniform sampler2D color_buffer;
layout(binding = 3) uniform sampler2D normal_buffer;
layout(binding = 4) uniform sampler2D depth_buffer;

void main() {
#if 0 // Cheap dFd approach
  GBufferSample gbuffer = sample_gbuffer(color_buffer, normal_buffer, depth_buffer);
  float depth_diff =
    abs(dFdx(gbuffer.depth)) +
    abs(dFdy(gbuffer.depth));
  float normal_diff =
    abs(length(dFdx(gbuffer.normal))) +
    abs(length(dFdy(gbuffer.normal)));
  if(depth_diff > 0.0004 || normal_diff > 0.5) {
    fragcolor.rgba = vec4(-99999.f);
  } else {
    fragcolor.rgba = vec4(0.f);
  }
#else // 4x depth+normal comparison
  vec2 texcoords = gl_FragCoord.xy * viewport_pixel_size.zw;
  float offx = thickness * viewport_pixel_size.z * 0.5f;
  float offy = thickness * viewport_pixel_size.w * 0.5f;

  float depth_left = texture(depth_buffer, vec2(texcoords.x - offx, texcoords.y)).r;
  float depth_right = texture(depth_buffer, vec2(texcoords.x + offx, texcoords.y)).r;
  float depth_top = texture(depth_buffer, vec2(texcoords.x, texcoords.y - offy)).r;
  float depth_bottom = texture(depth_buffer, vec2(texcoords.x, texcoords.y + offy)).r;

  vec3 normal_left = decodeNormal(texture(normal_buffer, vec2(texcoords.x - offx, texcoords.y)).xy);
  vec3 normal_right = decodeNormal(texture(normal_buffer, vec2(texcoords.x + offx, texcoords.y)).xy);
  vec3 normal_top = decodeNormal(texture(normal_buffer, vec2(texcoords.x, texcoords.y - offy)).xy);
  vec3 normal_bottom = decodeNormal(texture(normal_buffer, vec2(texcoords.x, texcoords.y + offy)).xy);

  float depth_diff = abs(depth_left - depth_right) + abs(depth_top - depth_bottom);
  float normal_diff = abs(length(normal_left - normal_right)) + abs(length(normal_top - normal_bottom));

  if(depth_diff > 0.0004f || normal_diff > 0.5f) {
    fragcolor.rgba = vec4(-99999.f);
  } else {
    fragcolor.rgba = vec4(0.f);
  }
#endif
}
