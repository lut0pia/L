layout(binding = 1) uniform sampler2D light_buffer;
layout(location = 0) out vec4 fragcolor;

void main() {
  vec2 texcoords = gl_FragCoord.xy*viewport_pixel_size.zw;
  vec3 color = texture(light_buffer, texcoords).rgb;
  color = pow(color, vec3(1.f/2.2f)); // Gamma correction
  fragcolor = vec4(deband(color), 1.f);
}
