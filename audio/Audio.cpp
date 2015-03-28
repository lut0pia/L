#include "Audio.h"

#if defined L_WINDOWS
#include <windows.h>
#endif

#include "../Exception.h"
#include "../macros.h"

using namespace L;

void Audio::playSound(const String& filename, size_t flags) {
#if defined L_WINDOWS
  PlaySound(filename.c_str(),NULL,SND_FILENAME);
#else
  throw Exception("Cannot play sound under this OS.");
#endif
}
