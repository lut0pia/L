#include "GamePlatform.h"

using namespace L;

Array<Handle<GamePlatform>> game_platforms;

GamePlatform::GamePlatform(const Symbol& name)
  : Handled<GamePlatform>(this), _name(name) {
  game_platforms.push(handle());
}

const Array<Handle<GamePlatform>>& GamePlatform::get_game_platforms() {
  return game_platforms;
}
