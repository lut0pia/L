#include "AudioListenerComponent.h"

#include "../engine/Engine.h"

using namespace L;

Vector3f AudioListenerComponent::_position, AudioListenerComponent::_right,
AudioListenerComponent::_forward, AudioListenerComponent::_up,
AudioListenerComponent::_velocity, AudioListenerComponent::_last_position;

void AudioListenerComponent::update() {
  _position = _transform->position();
  _right = _transform->right();
  _forward = _transform->forward();
  _up = _transform->up();
  _velocity = (_position-_last_position)*Engine::delta_seconds();
  _last_position = _position;
}
void AudioListenerComponent::update_components() {
  _transform = entity()->require_component<Transform>();
  _last_position = _transform->position();
}
void AudioListenerComponent::script_registration() {
  L_COMPONENT_BIND(AudioListenerComponent, "audio_listener");
}
