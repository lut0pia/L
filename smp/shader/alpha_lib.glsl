#include <shader/constants.glsl>

float gold_noise(in vec2 coordinate, in float seed) {
  float noise = fract(tan(distance(coordinate * (seed + PHI * 0.1f), vec2(PHI * 0.1f, PI * 0.1f))) * SQ2 * 10000.f);
  return isnan(noise) ? 0.f : noise;
}
float frag_noise() {
  return gold_noise(gl_FragCoord.xy, frame % 4);
}
bool alpha(float a) {
  return a < clamp(frag_noise(), 0.01f, 0.99f);
}
