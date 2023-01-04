#ifndef _PY_TASK_H_
#define _PY_TASK_H_

#include <Python.h> /* Must be first */
#include <stdbool.h>

bool S_Task_Init(void);
void S_Task_Shutdown(void);
void S_Task_Clear(void);
void S_Task_PyRegister(PyObject *module);
PyObject *S_Task_GetAll(void);
void S_Task_Flush(void);

#endif /* _PY_TASK_H_ */
