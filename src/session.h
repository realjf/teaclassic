#ifndef _SESSION_H_
#define _SESSION_H_

#include <stdbool.h>
#include <stdint.h>

#define MAX_ARGC (32)

struct SDL_RWops;

struct arg_desc {
    int argc;
    char *argv[MAX_ARGC + 1];
};

bool Session_Init(void);
void Session_Shutdown(void);
void Session_ServiceRequests(void);

bool Session_Save(struct SDL_RWops *stream);
void Session_RequestLoad(const char *path);

void Session_RequestPush(const char *script, int argc, char **argv);
void Session_RequestPop(int argc, char **argv);
void Session_RequestPopToRoot(int argc, char **argv);
void Session_RequestExec(const char *script, int argc, char **argv);
int Session_StackDepth(void);
uint64_t Session_ChangeTick(void);

#endif /* _SESSION_H_ */
