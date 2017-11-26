#version 330 core
#stage vertex
uniform vec3 l_pos;
uniform float l_rad;
uniform int l_type;

vec4 sphere() {
  const vec3 vertices[] = vec3[](
    vec3(0,0,1), vec3(0,1,1), vec3(1,0,1),
    vec3(0,1,0), vec3(1,1,0), vec3(0,1,1),
    vec3(1,0,0), vec3(1,0,1), vec3(1,1,0),
    vec3(0,1,1), vec3(1,1,0), vec3(1,0,1));

  int vertex_id = gl_VertexID%vertices.length();
  int quadrant = gl_VertexID/12;
  vec3 mult = vec3(1,1,1);
  if((quadrant&0x1)!=0){
      mult.x = -1;
      vertex_id = vertices.length()-vertex_id-1;
  }
  if((quadrant&0x2)!=0){
      mult.y = -1;
      vertex_id = vertices.length()-vertex_id-1;
  }
  if((quadrant&0x4)!=0){
      mult.z = -1;
      vertex_id = vertices.length()-vertex_id-1;
  }
  vec3 vertex = normalize(vertices[vertex_id]*mult);
  return viewProj * vec4(l_pos+vertex*l_rad*1.2f, 1.f);
}
void main() {
  const vec2 vertices[] = vec2[](
    vec2(-1.f,-1.f),vec2(3.f,-1.f),vec2(-1.f,3.f));
  switch(l_type) {
    case 0: case 2: gl_Position = vec4(vertices[gl_VertexID],0.f,1.f); break;
    case 1: gl_Position = sphere(); break;
  }
}

#stage fragment
out vec4 fragcolor;
uniform sampler2D color_buffer;
uniform sampler2D normal_buffer;
uniform sampler2D depth_buffer;
uniform vec3 l_pos;
uniform vec3 l_dir;
uniform vec3 l_color;
uniform float l_int;
uniform float l_rad;
uniform float l_in_ang;
uniform float l_out_ang;
uniform int l_type;

void main(){
  vec2 texcoords = gl_FragCoord.xy*viewport_pixel_size.zw;
  vec4 color_sample = texture(color_buffer,texcoords);
  vec3 color = color_sample.rgb;
  float metalness = color_sample.a;
  vec4 normal_sample = texture(normal_buffer,texcoords);
  vec3 normal = decodeNormal(normal_sample.xy);
  float roughness = normal_sample.z;
  float depth = texture(depth_buffer,texcoords).r;
  vec4 position_p = invViewProj * vec4(texcoords*2.f-1.f,depth*2.f-1.f,1.f);
  vec3 position = position_p.xyz/position_p.w;
  vec3 frag_to_light = (l_pos-position);
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
