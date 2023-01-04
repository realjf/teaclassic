#ifndef _PY_TRAVERSE_H_
#define _PY_TRAVERSE_H_

#include <Python.h> /* Must be included first */
#include <stdbool.h>
#include "../lib/public/khash.h"

__KHASH_TYPE(str, khint64_t, const char *)
__KHASH_PROTOTYPES(str, khint64_t, const char *)

__KHASH_TYPE(pobj, kh_cstr_t, PyObject *)
__KHASH_PROTOTYPES(pobj, kh_cstr_t, PyObject *)

bool S_Traverse_DF(PyObject *root, visitproc visit, void *user);
bool S_Traverse_PrintDF(PyObject *root);

bool S_Traverse_BF(PyObject *root, visitproc visit, void *user);
bool S_Traverse_PrintBF(PyObject *root);

bool S_Traverse_IndexQualnames(khash_t(str) * inout);
bool S_Traverse_ReferencesObj(PyObject *root, PyObject *obj, bool *out);


#endif /* _PY_TRAVERSE_H_ */
