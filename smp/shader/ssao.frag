layout(location = 0) out vec4 fragcolor;
 
layout(binding = 1) uniform sampler2D normal_buffer;
layout(binding = 2) uniform sampler2D depth_buffer;

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

float PHI = 1.61803398874989484820459 * 00000.1; // Golden Ratio   
float PI_  = 3.14159265358979323846264 * 00000.1; // PI
float SQ2 = 1.41421356237309504880169 * 10000.0; // Square Root of Two

float gold_noise(in vec2 coordinate, in float seed){
    return fract(tan(distance(coordinate*(seed+PHI), vec2(PHI, PI_)))*SQ2);
}

void main() {
  vec2 texcoords = gl_FragCoord.xy*viewport_pixel_size.zw;
  float depth = texture(depth_buffer, texcoords).r;
  vec4 position_p = invViewProj * vec4(texcoords*2.f-1.f, depth, 1.f);
  vec3 position = position_p.xyz/position_p.w;
  vec3 normal = decodeNormal(texture(normal_buffer, texcoords).xy);

  vec3 rand_vec  = normalize(vec3(
    gold_noise(texcoords,frame+0)-.5f,
    gold_noise(texcoords,frame+1)-.5f,
    gold_noise(texcoords,frame+2)-.5f));
  vec3 tangent   = normalize(rand_vec - normal * dot(rand_vec, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN       = mat3(tangent, bitangent, normal);

  const float radius = 1.f;
  float occlusion = 0;
  for(int i=0;i<samples;i++) {
    vec3 smp = TBN * sample_hemisphere[i]; // From tangent to world-space
    smp = position + smp * radius;
    vec4 projected_smp = viewProj * vec4(smp,1.f);
    projected_smp.xyz /= projected_smp.w;
    projected_smp.xyz = projected_smp.xyz * 0.5 + 0.5;
    
    float neighbor_depth = texture(depth_buffer, projected_smp.xy).r;
    float range_check = smoothstep(0.0, 1.0, radius / abs(depth - neighbor_depth));
    occlusion += (neighbor_depth > depth-0.00001f) ? (inv_samples*range_check) : 0.f;
  }
  occlusion = occlusion*0.7f+0.3f;
  fragcolor.rgba = vec4(occlusion,occlusion,occlusion,1);
}
