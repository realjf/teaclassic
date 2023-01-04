#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "../../tc_math.h"

#include <stdbool.h>
#include <stddef.h>

#define AUDIO_NUM_FG_CHANNELS (4)

struct map;
struct SDL_RWops;

enum playback_mode {
  MUSIC_MODE_LOOP,
  MUSIC_MODE_PLAYLIST,
  MUSIC_MODE_SHUFFLE,
};

bool Audio_Init(void);
void Audio_Shutdown(void);
bool Audio_PlayMusic(const char *name);
void Audio_PlayMusicFirst(void);
bool Audio_PlayPositionalEffect(const char *name, vec3_t pos);
bool Audio_PlayForegroundEffect(const char *name, bool interrupt, int channel);
size_t Audio_GetAllMusic(size_t maxout, const char *out[static maxout]);
const char *Audio_CurrMusic(void);
bool Audio_Effect_Add(vec3_t pos, const char *track);
void Audio_Pause(void);
void Audio_Resume(uint32_t dt);
void Audio_ClearState(void);
bool Audio_SaveState(struct SDL_RWops *stream);
bool Audio_LoadState(struct SDL_RWops *stream);

#endif /* _AUDIO_H_ */
