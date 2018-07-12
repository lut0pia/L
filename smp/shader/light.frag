layout(location = 0) out vec4 fragcolor;

layout(binding = 1) uniform Light {
  vec3 l_dir;
  vec3 l_color;
  float l_int;
  float l_rad;
  float l_in_ang;
  float l_out_ang;
  int l_type;
};
 
layout(binding = 2) uniform sampler2D color_buffer;
layout(binding = 3) uniform sampler2D normal_buffer;
layout(binding = 4) uniform sampler2D depth_buffer;

void main() {
  vec3 light_pos = model[3].xyz;
  vec3 world_dir = mat3(model) * l_dir;
  vec2 texcoords = gl_FragCoord.xy*viewport_pixel_size.zw;
  vec4 color_sample = texture(color_buffer,texcoords);
  vec3 color = color_sample.rgb;
  float metalness = color_sample.a;
  vec4 normal_sample = texture(normal_buffer,texcoords);
  vec3 normal = decodeNormal(normal_sample.xy);
  float roughness = normal_sample.z;
  float depth = texture(depth_buffer,texcoords).r;
  vec4 position_p = invViewProj * vec4(texcoords*2.f-1.f,depth,1.f);
  vec3 position = position_p.xyz/position_p.w;
  vec3 frag_to_light = (light_pos-position);
  vec3 to_light_dir = (l_type==0) ? -l_dir : normalize(frag_to_light);
  vec3 view_dir = normalize(eye.xyz - position);
  vec3 halfway = normalize(view_dir+to_light_dir);
  float dist = length(frag_to_light);
  float att = light_attenuation(dist,l_rad,l_int);
  if(l_type == 2 && dot(l_dir, -to_light_dir)<l_in_ang) att = 0.f;
  else if(l_type == 0) att = l_int;
  vec3 radiance = l_color*att;
  vec3 F0 = mix(vec3(0.04f),color,metalness);
  vec3 F = fresnel_schlick(max(dot(halfway,view_dir),0.f),F0);
  float NDF = distribution_GGX(normal,halfway,roughness);
  float G = geometry_smith(normal,view_dir,to_light_dir,roughness);
  vec3 spec = specular(NDF,G,F,normal,view_dir,to_light_dir);
  vec3 diff = (vec3(1.0)-F) * (1.f-metalness) * color;
  float NdotL = max(dot(normal, to_light_dir), 0.0);
  fragcolor.rgb = (diff / PI + spec) * radiance * NdotL;
  fragcolor.a = 1.f;
}
