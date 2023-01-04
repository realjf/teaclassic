#ifndef _GL_PERF_H_
#define _GL_PERF_H_

#include "../perf.h"
#include <GL/glew.h>

#include "gl_assert.h"

#ifndef NDEBUG

extern bool g_trace_gpu;

#define GL_GPU_PERF_PUSH(name)                  \
    do{                                         \
        if(!g_trace_gpu)                        \
            break;                              \
        GLuint cookie;                          \
        glGenQueries(1, &cookie);               \
        glQueryCounter(cookie, GL_TIMESTAMP);   \
        GL_ASSERT_OK(); \
        Perf_PushGPU(name, cookie);             \
    }while(0)

#define GL_GPU_PERF_POP()                       \
    do{                                         \
        if(!g_trace_gpu)                        \
            break;                              \
        GLuint cookie;                          \
        glGenQueries(1, &cookie);               \
        glQueryCounter(cookie, GL_TIMESTAMP);   \
        GL_ASSERT_OK(); \
        Perf_PopGPU(cookie);                    \
    }while(0)

#define GL_PERF_ENTER()                         \
    do{                                         \
        Perf_Push(__func__);                    \
        GL_GPU_PERF_PUSH(__func__);             \
    }while(0)

#define GL_PERF_RETURN(...)                     \
    do{                                         \
        Perf_Pop();                             \
        GL_GPU_PERF_POP();                      \
        return (__VA_ARGS__);                   \
    }while(0)

#define GL_PERF_RETURN_VOID()                   \
    do{                                         \
        Perf_Pop();                             \
        GL_GPU_PERF_POP();                      \
        return;                                 \
    }while(0)

#else

#define GL_GPU_PERF_PUSH(name)
#define GL_GPU_PERF_POP()

#define GL_PERF_ENTER()
#define GL_PERF_RETURN(...) do {return (__VA_ARGS__); } while(0)
#define GL_PERF_RETURN_VOID(...) do { return; } while(0)

#endif //NDEBUG


#endif /* _GL_PERF_H_ */
