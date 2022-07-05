#include <shader/constants.glsl>

float light_attenuation(float dist, float radius, float intensity) {
  float num = clamp(1.f-pow(dist/radius,4.f),0.f,1.f);
  return intensity*num*num/(dist*dist+1.f);
}
vec3 fresnel_schlick(float cos_theta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(1.0 - cos_theta, 5.0);
}
float distribution_GGX(vec3 N, vec3 H, float roughness) {
  float a = roughness*roughness;
  float a2 = a*a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;
  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;
  return nom / denom;
}
float geometry_schlick_GGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r*r) / 8.0;
  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;
  return nom / denom;
}
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = geometry_schlick_GGX(NdotV, roughness);
  float ggx1 = geometry_schlick_GGX(NdotL, roughness);
  return ggx1 * ggx2;
}
vec3 specular(float NDF, float G, vec3 F, vec3 N, vec3 V, vec3 L) {
  vec3 nominator = NDF * G * F;
  float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001f;
  return nominator / denominator;
}

vec4 cel_shading(GBufferSample gbuffer, vec3 light_color, vec3 to_light_dir, float dist_factor) {
  vec4 result = vec4(0.f);
  vec3 view_dir = normalize(eye.xyz - gbuffer.position);
  vec3 halfway = normalize(view_dir + to_light_dir);

  float NdotL = dot(gbuffer.normal, to_light_dir);
  float factor = NdotL > -0.1f ? 0.7f : 0.01; // Normal lighting
  factor *= 1.f - min(floor(dist_factor * 2.f) / 2.f, 1.f); // Distance attenuation

  if(gbuffer.roughness < 0.75f && dot(halfway, gbuffer.normal) > 1.f - gbuffer.roughness * 0.25f) {
    factor = 1.f; // Specular sort of
  }

  vec3 radiance = light_color * gbuffer.color;
  result.rgb = radiance * factor;
  return result;
}
