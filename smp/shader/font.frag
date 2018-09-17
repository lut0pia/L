layout(location = 0) in vec2 ftexcoords;

layout(location = 0) out vec4 ocolor;

layout(binding = 1) uniform Parameters {
  vec4 color;
};
layout(binding = 2) uniform sampler2D atlas;

void main() {
  ocolor = color;
  const float dist = texture(atlas, ftexcoords).r;
  const vec2 tex_size = textureSize(atlas, 0);
  const float range = dFdx(ftexcoords.x)*tex_size.x*0.015f; // Change to adjust crispness
  const float thickness_border = 0.05f;
  const float thickness = 0.5f;
  const float total_thickness = thickness + thickness_border;
  ocolor.rgb *= smoothstep(range, 0.f, dist-thickness+range*0.5f);
  ocolor.a *= smoothstep(range, 0.f, dist-total_thickness+range*0.5f);
}
