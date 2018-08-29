layout(location = 0) in vec2 ftexcoords;

layout(location = 0) out vec4 ocolor;

layout(binding = 1) uniform Parameters {
  vec4 color;
};
layout(binding = 2) uniform sampler2D atlas;

void main() {
  ocolor = color;
  ocolor.a *= texture(atlas, ftexcoords).r;
}
