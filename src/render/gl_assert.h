#ifndef _GL_ASSERT_H_
#define _GL_ASSERT_H_

#include <assert.h>
#include <stdio.h>

#ifndef NDEBUG

#define GL_ASSERT_OK()                                  \
    do {                                                \
        GLenum error = glGetError();                    \
        if(error != GL_NO_ERROR)                        \
            fprintf(stderr, "%s:%d OpenGL error: %x\n", \
            __FILE__, __LINE__, error);                 \
        assert(error == GL_NO_ERROR);                   \
    }while(0)

#else

#define GL_ASSERT_OK() /* no-op */

#endif


#endif /* _GL_ASSERT_H_ */
