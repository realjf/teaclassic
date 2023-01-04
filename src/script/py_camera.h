#ifndef _PY_CAMERA_H_
#define _PY_CAMERA_H_

#include <Python.h> /* must be first */
#include <stdbool.h>

void S_Camera_PyRegister(PyObject *module);
bool S_Camera_Init(void);
void S_Camera_Shutdown(void);
void S_Camera_Clear(void);

PyObject *S_Camera_GetActive(void);
bool S_Camera_SetActive(PyObject *cam);

#endif /* _PY_CAMERA_H_ */
