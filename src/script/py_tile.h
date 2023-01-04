#ifndef _PY_TILE_H_
#define _PY_TILE_H_

#include <Python.h> /* must be first */

struct tile;
struct tile_desc;

void S_Tile_PyRegister(PyObject *module);
const struct tile *S_Tile_GetTile(PyObject *tile_obj);
PyObject *S_Tile_New(struct tile_desc *td);

#endif /* _PY_TILE_H_ */
