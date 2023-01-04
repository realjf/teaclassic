#ifndef _AL_EFFECT_H_
#define _AL_EFFECT_H_

#include <stdbool.h>
#include <stdint.h>

struct SDL_RWops;

bool Audio_Effect_Init(void);
void Audio_Effect_Shutdown(void);
float Audio_EffectVolume(void);
void Audio_EffectClearState(void);
void Audio_EffectPause(void);
void Audio_EffectResume(uint32_t dt);
bool Audio_EffectSaveState(struct SDL_RWops *stream);
bool Audio_EffectLoadState(struct SDL_RWops *stream);

#endif /* _AL_EFFECT_H_ */
