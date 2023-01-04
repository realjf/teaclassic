#ifndef _PY_ENTITY_H_
#define _PY_ENTITY_H_

#include <Python.h> /* Must be first */
#include <stdbool.h>
#include <stdint.h>

bool S_Entity_Init(void);
void S_Entity_Shutdown(void);
void S_Entity_Clear(void);
void S_Entity_PyRegister(PyObject *module);
bool S_Entity_Check(PyObject *obj);
/* Returned list has a stolen reference to each object */
PyObject *S_Entity_GetLoaded(void);

#endif /* _PY_ENTITY_H_ */
