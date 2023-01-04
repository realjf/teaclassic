#ifndef _AL_ASSERT_H_
#define _AL_ASSERT_H_

#include "al_private.h"

#include <assert.h>
#include <AL/al.h>

#ifndef NDEBUG

#define AL_ASSERT_OK()                                                  \
    do {                                                                \
        ALenum error = alGetError();                                    \
        if (error != AL_NO_ERROR)                                       \
            fprintf(stderr, "%s:%d OpenAL error: %x [%s]\n",            \
                    __FILE__, __LINE__, error, Audio_ErrString(error)); \
        assert(error == AL_NO_ERROR);                                   \
    } while (0)
#else

#define AL_ASSERT_OK() /* no-op */

#endif

#endif /* _AL_ASSERT_H_ */
