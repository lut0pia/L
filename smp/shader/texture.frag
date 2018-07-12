layout(location = 0) in vec2 ftexcoords;

layout(location = 0) out vec4 color;

layout(binding = 1) uniform sampler2D tex;

void main() {
  color = texture(tex, ftexcoords);
}
