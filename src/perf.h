#ifndef _PERF_H_
#define _PERF_H_

#include <stdbool.h>
#include <stddef.h>
#include <SDL_thread.h>

#ifndef NDEBUG

#define PERF_ENTER()         \
    do {                     \
        Perf_Push(__func__); \
    } while (0)

#define PERF_RETURN(...)      \
    do {                      \
        Perf_Pop();           \
        return (__VA_ARGS__); \
    } while (0)

#define PERF_RETURN_VOID() \
    do {                   \
        Perf_Pop();        \
        return;            \
    } while (0)

#define PERF_PUSH(name) \
    Perf_Push(name)

#define PERF_POP() \
    Perf_Pop()

#else

#define PERF_ENTER()
#define PERF_RETURN(...)      \
    do {                      \
        return (__VA_ARGS__); \
    } while (0)
#define PERF_RETURN_VOID(...) \
    do {                      \
        return;               \
    } while (0)
#define PERF_PUSH(name)
#define PERF_POP()

#endif

#define NFRAMES_LOGGED (5)

struct perf_info {
    char threadname[64];
    size_t nentries;
    struct {
        const char *funcname; /* borrowed */
        uint64_t pc_delta;
        double ms_delta;
        int parent_idx;
    } entries[];
};

void Perf_Push(const char *name);
void Perf_Pop(void);

void Perf_PushGPU(const char *name, uint32_t cookie);
void Perf_PopGPU(uint32_t cookie);

/* Note that due to buffering of the frame timing data, the statistics
 * reported will be from NFRAMES_LOGGED ago. The reason for this is that
 * the GPU may be lagging a couple of frames behind the CPU. We want to get
 * far enough ahead so that the GPU is finished with the frame we're
 * getting the statistics for. This way querying the GPU timestamps doesn't
 * cause a CPU<->GPU synch, which would negatively impact performance.
 */

/* This returns an array of perf_info structs (one for each thread). They
 * must be 'free'd by the caller. */
size_t Perf_Report(size_t maxout, struct perf_info **out);
uint32_t Perf_LastFrameMS(void);
uint32_t Perf_CurrFrameMS(void);

/* The following can only be called from the main thread, making sure that
 * none of the other threads are touching the Perf_ API concurrently */
bool Perf_Init(void);
void Perf_Shutdown(void);
bool Perf_RegisterThread(SDL_threadID tid, const char *name);
void Perf_BeginTick(void);
void Perf_FinishTick(void);

#endif /* _PERF_H_ */
