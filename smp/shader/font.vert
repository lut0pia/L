layout(location = 0) in vec2 vpositionfont; // Shitty names until actual reflection
layout(location = 1) in vec2 vtexcoordsfont;

layout(location = 0) out vec2 ftexcoords;

void main() {
  ftexcoords = vtexcoordsfont;
  gl_Position = model * vec4(vpositionfont,0.f,1.f);
}
