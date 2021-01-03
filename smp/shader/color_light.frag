layout(binding = 1) uniform Parameters {
  vec4 color;
};

layout(location = 0) out vec4 ocolor;

void main() {
  ocolor = color;
}
