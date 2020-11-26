layout(location = 0) out vec4 fragcolor;

layout(binding = 1) uniform Parameters {
  vec4 color;
  float radius;
};

layout(binding = 2) uniform sampler2D color_buffer;
layout(binding = 3) uniform sampler2D normal_buffer;
layout(binding = 4) uniform sampler2D depth_buffer;

const int samples = 16;
const float inv_samples = 1.f/samples;
vec3 sample_hemisphere[samples] = {
    vec3( 0.5381, 0.1856, 0.4319), vec3( 0.1379, 0.2486, 0.4430),
    vec3( 0.3371, 0.5679, 0.0057), vec3(-0.6999,-0.0451, 0.0019),
    vec3( 0.0689,-0.1598, 0.8547), vec3( 0.0560, 0.0069, 0.1843),
    vec3(-0.0146, 0.1402, 0.0762), vec3( 0.0100,-0.1924, 0.0344),
    vec3(-0.3577,-0.5301, 0.4358), vec3(-0.3169, 0.1063, 0.0158),
    vec3( 0.0103,-0.5869, 0.0046), vec3(-0.0897,-0.4940, 0.3287),
    vec3( 0.7119,-0.0154, 0.0918), vec3(-0.0533, 0.0596, 0.5411),
    vec3( 0.0352,-0.0631, 0.5460), vec3(-0.4776, 0.2847, 0.0271)
};

void main() {
  GBufferSample gbuffer = sample_gbuffer(color_buffer, normal_buffer, depth_buffer);
  vec2 texcoords = gl_FragCoord.xy*viewport_pixel_size.zw;
  float depth = gbuffer.linear_depth;
  vec3 position = gbuffer.position;
  vec3 normal = gbuffer.normal;

  vec3 rand_vec  = normalize(vec3(
    gold_noise(texcoords,frame+0)-.5f,
    gold_noise(texcoords,frame+1)-.5f,
    gold_noise(texcoords,frame+2)-.5f));
  vec3 tangent   = normalize(rand_vec - normal * dot(rand_vec, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN       = mat3(tangent, bitangent, normal);

  float occlusion = 1.f;
  for(int i=0;i<samples;i++) {
    vec3 smp = TBN * (sample_hemisphere[i]+vec3(0,0,.01f)); // From tangent to world-space
    smp = position + smp * radius;
    vec4 projected_smp = viewProj * vec4(smp, 1.f);
    projected_smp.xyz /= projected_smp.w;
    float neighbor_depth_target = linearize_depth(projected_smp.z);
    projected_smp.xyz = projected_smp.xyz * 0.5 + 0.5;

    float neighbor_depth = linearize_depth(texture(depth_buffer, coords_correction(projected_smp.xy)).r);
    float range_check = smoothstep(0.9f, 1.f, radius / abs(depth - neighbor_depth));
    occlusion -= (neighbor_depth < neighbor_depth_target) ? (inv_samples*range_check) : 0.f;
  }
  fragcolor.rgba = vec4(gbuffer.color*occlusion*color.rgb, 1.f);
}
