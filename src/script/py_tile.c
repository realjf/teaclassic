#include "py_tile.h"
#include "../game/public/game.h"
#include "../lib/public/SDL_vec_rwops.h"
#include "../map/public/map.h"
#include "../map/public/tile.h"
#include "py_pickle.h"

#include <structmember.h>

#define CHK_TRUE(_pred, _label)                                                \
  do {                                                                         \
    if (!(_pred))                                                              \
      goto _label;                                                             \
  } while (0)

typedef struct {
  PyObject_HEAD struct tile tile;
} PyTileObject;

static int PyTile_init(PyTileObject *self, PyObject *args);
static PyObject *PyTile_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

static PyObject *PyTile_get_top_left_height(PyTileObject *self, void *closure);
static PyObject *PyTile_get_top_right_height(PyTileObject *self, void *closure);
static PyObject *PyTile_get_bot_left_height(PyTileObject *self, void *closure);
static PyObject *PyTile_get_bot_right_height(PyTileObject *self, void *closure);

static PyObject *PyTile_pickle(PyTileObject *self, PyObject *args,
                               PyObject *kwargs);
static PyObject *PyTile_unpickle(PyObject *cls, PyObject *args,
                                 PyObject *kwargs);

/*****************************************************************************/
/* STATIC VARIABLES                                                          */
/*****************************************************************************/

#define BASE (offsetof(PyTileObject, tile))
static PyMemberDef PyTile_members[] = {
    {"pathable", T_INT, BASE + offsetof(struct tile, pathable), 0,
     "Whether or not units can travel through this tile."},
    {"type", T_INT, BASE + offsetof(struct tile, type), 0,
     "Integer value specifying whether this tile is a ramp, which direction it "
     "faces, etc."},
    {"base_height", T_INT, BASE + offsetof(struct tile, base_height), 0,
     "The height level of the bottom plane of the tile."},
    {"top_mat_idx", T_INT, BASE + offsetof(struct tile, top_mat_idx), 0,
     "Material index for the top face of the tile."},
    {"sides_mat_idx", T_INT, BASE + offsetof(struct tile, sides_mat_idx), 0,
     "Material index for the side faces of the tile."},
    {"ramp_height", T_INT, BASE + offsetof(struct tile, ramp_height), 0,
     "The height of the top edge of the ramp or corner above the base_height."},
    {"blend_mode", T_INT, BASE + offsetof(struct tile, blend_mode), 0,
     "The mode which determines how this tile's texture is blended with "
     "adjacent tiles' textures."},
    {"blend_normals", T_UBYTE, BASE + offsetof(struct tile, blend_normals), 0,
     "A boolean which determines if this tile's normals are averaged together "
     "with adjacent normals "
     "to create a 'smooth' terrain look."},
    {NULL} /* Sentinel */
};
#undef BASE

static PyGetSetDef PyTile_getset[] = {
    {"top_left_height", (getter)PyTile_get_top_left_height, NULL,
     "The height of the top left corner.", NULL},
    {"top_right_height", (getter)PyTile_get_top_right_height, NULL,
     "The height of the top right corner.", NULL},
    {"bot_left_height", (getter)PyTile_get_bot_left_height, NULL,
     "The height of the bot left corner.", NULL},
    {"bot_right_height", (getter)PyTile_get_bot_right_height, NULL,
     "The height of the bot right corner.", NULL},
    {NULL} /* Sentinel */
};

static PyMethodDef PyTile_methods[] = {
    {"__pickle__", (PyCFunction)PyTile_pickle, METH_KEYWORDS,
     "Serialize a Permafrost Engine tile to a string."},

    {"__unpickle__", (PyCFunction)PyTile_unpickle,
     METH_VARARGS | METH_KEYWORDS | METH_CLASS,
     "Create a new tc.Tile instance from a string earlier returned from a "
     "__pickle__ method."
     "Returns a tuple of the new instance and the number of bytes consumed "
     "from the stream."},

    {NULL} /* Sentinel */
};

static PyTypeObject PyTile_type = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "tc.Tile",
    .tp_basicsize = sizeof(PyTileObject),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "Map tile representation for Permafrost Engine maps.",
    .tp_members = PyTile_members,
    .tp_methods = PyTile_methods,
    .tp_getset = PyTile_getset,
    .tp_init = (initproc)PyTile_init,
    .tp_new = PyTile_new,
};

/*****************************************************************************/
/* STATIC FUNCTIONS                                                          */
/*****************************************************************************/

static int PyTile_init(PyTileObject *self, PyObject *args) {
  self->tile.pathable = true;
  self->tile.type = TILETYPE_FLAT;
  self->tile.base_height = 0;
  self->tile.ramp_height = 0;
  self->tile.top_mat_idx = 0;
  self->tile.sides_mat_idx = 1;
  self->tile.blend_mode = BLEND_MODE_BLUR;
  self->tile.blend_normals = true;

  return 0;
}

static PyObject *PyTile_new(PyTypeObject *type, PyObject *args,
                            PyObject *kwds) {
  PyObject *self = type->tp_alloc(type, 0);
  return self;
}

static PyObject *PyTile_get_top_left_height(PyTileObject *self, void *closure) {
  return Py_BuildValue("i", M_Tile_NWHeight(&self->tile));
}

static PyObject *PyTile_get_top_right_height(PyTileObject *self,
                                             void *closure) {
  return Py_BuildValue("i", M_Tile_NEHeight(&self->tile));
}

static PyObject *PyTile_get_bot_left_height(PyTileObject *self, void *closure) {
  return Py_BuildValue("i", M_Tile_SWHeight(&self->tile));
}

static PyObject *PyTile_get_bot_right_height(PyTileObject *self,
                                             void *closure) {
  return Py_BuildValue("i", M_Tile_SEHeight(&self->tile));
}

static PyObject *PyTile_pickle(PyTileObject *self, PyObject *args,
                               PyObject *kwargs) {
  bool status;
  PyObject *ret = NULL;

  SDL_RWops *stream = TCSDL_VectorRWOps();
  CHK_TRUE(stream, fail_alloc);

  PyObject *attrs = Py_BuildValue(
      "iiiiiiii", self->tile.pathable, self->tile.type, self->tile.base_height,
      self->tile.ramp_height, self->tile.top_mat_idx, self->tile.sides_mat_idx,
      self->tile.blend_mode, self->tile.blend_normals);
  status = S_PickleObjgraph(attrs, stream);
  Py_DECREF(attrs);
  CHK_TRUE(status, fail_pickle);
  ret = PyString_FromStringAndSize(TCSDL_VectorRWOpsRaw(stream),
                                   SDL_RWsize(stream));

fail_pickle:
  SDL_RWclose(stream);
fail_alloc:
  return ret;
}

static PyObject *PyTile_unpickle(PyObject *cls, PyObject *args,
                                 PyObject *kwargs) {
  PyObject *ret = NULL;
  const char *str;
  Py_ssize_t len;
  int status;
  char tmp;

  if (!PyArg_ParseTuple(args, "s#", &str, &len)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be a single string.");
    goto fail_args;
  }

  SDL_RWops *stream = SDL_RWFromConstMem(str, len);
  CHK_TRUE(stream, fail_args);

  PyObject *attrs = S_UnpickleObjgraph(stream);
  SDL_RWread(stream, &tmp, 1, 1); /* consume NULL byte */
  CHK_TRUE(attrs, fail_unpickle);

  PyObject *tile_args = PyTuple_New(0);
  PyTileObject *tileobj =
      (PyTileObject *)((PyTypeObject *)cls)
          ->tp_new((struct _typeobject *)cls, tile_args, NULL);
  CHK_TRUE(tileobj, fail_tile);

  int pathable;
  int blend_normals;

  if (!PyArg_ParseTuple(attrs, "iiiiiiii", &pathable, &tileobj->tile.type,
                        &tileobj->tile.base_height, &tileobj->tile.ramp_height,
                        &tileobj->tile.top_mat_idx,
                        &tileobj->tile.sides_mat_idx, &tileobj->tile.blend_mode,
                        &blend_normals)) {
    goto fail_tile;
  }
  tileobj->tile.pathable = !!pathable;
  tileobj->tile.blend_normals = !!blend_normals;

  Py_ssize_t nread = SDL_RWseek(stream, 0, RW_SEEK_CUR);
  ret = Py_BuildValue("(Oi)", tileobj, (int)nread);

fail_tile:
  Py_XDECREF(tileobj);
  Py_XDECREF(tile_args);
fail_unpickle:
  Py_XDECREF(attrs);
  SDL_RWclose(stream);
fail_args:
  return ret;
}

/*****************************************************************************/
/* EXTERN FUNCTIONS                                                          */
/*****************************************************************************/

void S_Tile_PyRegister(PyObject *module) {
  if (PyType_Ready(&PyTile_type) < 0)
    return;
  Py_INCREF(&PyTile_type);
  PyModule_AddObject(module, "Tile", (PyObject *)&PyTile_type);
}

const struct tile *S_Tile_GetTile(PyObject *tile_obj) {
  if (!PyObject_IsInstance(tile_obj, (PyObject *)&PyTile_type))
    return NULL;

  return &((PyTileObject *)tile_obj)->tile;
}

PyObject *S_Tile_New(struct tile_desc *td) {
  struct tile tile;
  if (!G_GetTile(td, &tile))
    return NULL;

  PyTileObject *ret =
      (PyTileObject *)PyObject_CallFunction((PyObject *)&PyTile_type, "()");
  if (!ret)
    return NULL;

  ret->tile = tile;
  return (PyObject *)ret;
}
