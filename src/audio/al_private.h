#ifndef _AL_PRIVATE_H_
#define _AL_PRIVATE_H_

#include "../tc_math.h"
#include <AL/al.h>

#define HEARING_RANGE (165.0f)

const char *Audio_ErrString(ALenum err);
bool Audio_GetEffectBuffer(const char *name, ALint *out);
const char *Audio_GetEffectName(ALuint buffer);
vec2_t Audio_ListenerPosXZ(void);
float Audio_BufferDuration(ALuint buffer);
void Audio_SetForegroundEffectVolume(float gain);

#endif /* _AL_PRIVATE_H_ */
