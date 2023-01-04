#ifndef _TASK_H_
#define _TASK_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

struct taskret;
struct future;

typedef struct result (*task_t)(void *);

/* The following may only be called from task context
 * (i.e. from the body of a task function) */

uint32_t Task_Create(int prio, task_t code, void *arg, struct future *result, int flags);
bool Task_Wait(uint32_t child_tid);
uint32_t Task_MyTid(void);
uint32_t Task_ParentTid(void);
void Task_Yield(void);
void Task_Send(uint32_t tid, void *msg, size_t msglen, void *reply, size_t replylen);
void Task_Receive(uint32_t *tid, void *msg, size_t msglen);
void Task_Reply(uint32_t tid, void *reply, size_t replylen);
void *Task_AwaitEvent(int event, int *source);
void Task_SetDestructor(void (*destructor)(void *), void *darg);
void Task_Sleep(int ms);
void Task_Register(const char *name);
void Task_Unregister(void);
uint32_t Task_WhoIs(const char *name, bool blocking);

/* The following may only be called from the main thread */

void Task_CreateServices(void);

#endif /* _TASK_H_ */
