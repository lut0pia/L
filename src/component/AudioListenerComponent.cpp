#include "AudioListenerComponent.h"

using namespace L;

void AudioListenerComponent::update() {
  Audio::listener_position(_transform->position());
  Audio::listener_orientation(_transform->forward(),_transform->up());
  Audio::listener_velocity((_transform->position()-_last_position)*Engine::deltaSeconds());
  _last_position = _transform->position();
}
void AudioListenerComponent::updateComponents() {
  _transform = entity()->requireComponent<Transform>();
  _last_position = _transform->position();
}