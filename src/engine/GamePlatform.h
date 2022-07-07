#pragma once

#include "../container/Array.h"
#include "../container/Handle.h"
#include "../text/String.h"
#include "../text/Symbol.h"
#include "../time/Date.h"

namespace L {
  struct GameAchievement {
    uint32_t index;
    String name;
    bool is_unlocked;
    Date unlock_time;
  };

  class GamePlatform : Handled<GamePlatform> {
  protected:
    Symbol _name;
    bool _overlay_active = false;
    String _player_name;
    Array<GameAchievement> _achievements;

  public:
    GamePlatform(const Symbol& name);

    // Achievements
    virtual void unlock_achievement(const String& name) = 0;
    virtual void clear_achievement(const String& name) = 0;

    const Symbol& get_name() const { return _name; }
    const String& get_player_name() const { return _player_name; }
    const Array<GameAchievement>& get_achievements() const { return _achievements; }

    static const Array<Handle<GamePlatform>>& get_game_platforms();
  };
}
