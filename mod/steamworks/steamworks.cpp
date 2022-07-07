#include <L/src/dev/debug.h>
#include <L/src/engine/Engine.h>
#include <L/src/engine/GamePlatform.h>
#include <steam/steam_api.h>

using namespace L;

static class SteamworksGamePlatform* game_platform;

class SteamworksGamePlatform : GamePlatform {
private:
  STEAM_CALLBACK(SteamworksGamePlatform, OnGameOverlayActivated, GameOverlayActivated_t);

public:
  SteamworksGamePlatform()
    : GamePlatform("steamworks") {
    _player_name = SteamFriends()->GetPersonaName();
    refresh_achievements();
  }

  // Achievements

  virtual void unlock_achievement(const String& name) override {
    SteamUserStats()->SetAchievement(name.begin());
    SteamUserStats()->StoreStats();
    refresh_achievements();
  }

  virtual void clear_achievement(const String& name) override {
    SteamUserStats()->ClearAchievement(name.begin());
    SteamUserStats()->StoreStats();
    refresh_achievements();
  }

  void refresh_achievements() {
    _achievements.clear();
    const uint32_t num_achievements = SteamUserStats()->GetNumAchievements();
    for(uint32_t i = 0; i < num_achievements; i++) {
      _achievements.push();
      GameAchievement& achievement = _achievements.back();
      achievement.index = i;
      achievement.name = SteamUserStats()->GetAchievementName(i);

      uint32_t unlocked_time;
      SteamUserStats()->GetAchievementAndUnlockTime(achievement.name, &achievement.is_unlocked, &unlocked_time);
      if(achievement.is_unlocked) {
        achievement.unlock_time = Date(unlocked_time);
      }
    }
  }
};

void SteamworksGamePlatform::OnGameOverlayActivated(GameOverlayActivated_t* pCallback) {
  _overlay_active = pCallback->m_bActive;
}

void steamworks_module_update() {
  SteamAPI_RunCallbacks();
}

void steamworks_module_init() {
#if L_RLS && defined(L_STEAM_APPID)
  if(SteamAPI_RestartAppIfNecessary(L_STEAM_APPID)) {
    exit(1);
  }
#endif
  if(!SteamAPI_Init()) {
    warning("steamworks: Failed to init");
    return;
  }

  game_platform = new SteamworksGamePlatform;
  Engine::add_update(steamworks_module_update);
}
