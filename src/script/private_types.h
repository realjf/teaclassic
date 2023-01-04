#ifndef _PRIVATE_TYPES_H_
#define _PRIVATE_TYPES_H_

#include <Python.h>
#include <../Objects/stringlib/stringdefs.h>

/* from Objects/descrobject.c */
typedef struct {
    PyObject_HEAD
        PyObject *dict;
} proxyobject;

/* from Objects/descrobject.c */
typedef struct {
    PyObject_HEAD
        PyWrapperDescrObject *descr;
    PyObject *self;
} wrapperobject;

/* from Objects/typeobject.c */
typedef struct {
    PyObject_HEAD
        PyTypeObject *type;
    PyObject *obj;
    PyTypeObject *obj_type;
} superobject;

/* from Objects/rangeobject.c */
typedef struct {
    PyObject_HEAD long start;
    long step;
    long len;
} rangeobject;

/* from Objects/funcobject.c */
typedef struct {
    PyObject_HEAD
        PyObject *sm_callable;
} staticmethod;

/* from Objects/bufferobject.c */
typedef struct {
    PyObject_HEAD
        PyObject *b_base;
    void *b_ptr;
    Py_ssize_t b_size;
    Py_ssize_t b_offset;
    int b_readonly;
    long b_hash;
} PyBufferObject;

/* from Objects/descrobject.c */
typedef struct {
    PyObject_HEAD
        PyObject *prop_get;
    PyObject *prop_set;
    PyObject *prop_del;
    PyObject *prop_doc;
    int getter_doc;
} propertyobject;

/* from Objects/enumobject.c */
typedef struct {
    PyObject_HEAD
        Py_ssize_t en_index; /* current index of enumeration */
    PyObject *en_sit;        /* secondary iterator of enumeration */
    PyObject *en_result;     /* result tuple  */
    PyObject *en_longindex;  /* index for sequences >= PY_SSIZE_T_MAX */
} enumobject;

/* from Objects/enumobject.c */
typedef struct {
    PyObject_HEAD
        Py_ssize_t index;
    PyObject *seq;
} reversedobject;

/* from Modules/zipimport.c */
struct _zipimporter {
    PyObject_HEAD
        PyObject *archive; /* pathname of the Zip archive */
    PyObject *prefix;      /* file prefix: "a/sub/directory/" */
    PyObject *files;       /* dict with file info {path: toc_entry} */
};

/* from Objects/dictobject.c */
typedef struct {
    PyObject_HEAD
        PyDictObject *dv_dict;
} dictviewobject;

/* from Objects/iterobject.c */
typedef struct {
    PyObject_HEAD
        PyObject *it_callable; /* Set to NULL when iterator is exhausted */
    PyObject *it_sentinel;     /* Set to NULL when iterator is exhausted */
} calliterobject;

/* from Objects/iterobject.c */
typedef struct {
    PyObject_HEAD long it_index;
    PyObject *it_seq; /* Set to NULL when iterator is exhausted */
} seqiterobject;

/* from Objects/dictobject.c */
typedef struct {
    PyObject_HEAD
        PyDictObject *di_dict; /* Set to NULL when iterator is exhausted */
    Py_ssize_t di_used;
    Py_ssize_t di_pos;
    PyObject *di_result; /* reusable result tuple for iteritems */
    Py_ssize_t len;
} dictiterobject;

/* from Objects/setobject.c */
typedef struct {
    PyObject_HEAD
        PySetObject *si_set; /* Set to NULL when iterator is exhausted */
    Py_ssize_t si_used;
    Py_ssize_t si_pos;
    Py_ssize_t len;
} setiterobject;

/* from Objects/stringlib/string_format.h */

typedef struct {
    STRINGLIB_CHAR *ptr;
    STRINGLIB_CHAR *end;
} SubString;

typedef struct {
    /* the entire string we're parsing.  we assume that someone else
       is managing its lifetime, and that it will exist for the
       lifetime of the iterator.  can be empty */
    SubString str;

    /* pointer to where we are inside field_name */
    STRINGLIB_CHAR *ptr;
} FieldNameIterator;

typedef struct {
    PyObject_HEAD

        STRINGLIB_OBJECT *str;

    FieldNameIterator it_field;
} fieldnameiterobject;

typedef struct {
    SubString str;
} MarkupIterator;

typedef struct {
    PyObject_HEAD

        STRINGLIB_OBJECT *str;

    MarkupIterator it_markup;
} formatteriterobject;

/* from Modules/operator.c */

typedef struct {
    PyObject_HEAD
        Py_ssize_t nitems;
    PyObject *item;
} itemgetterobject;

typedef struct {
    PyObject_HEAD
        Py_ssize_t nattrs;
    PyObject *attr;
} attrgetterobject;

typedef struct {
    PyObject_HEAD
        PyObject *name;
    PyObject *args;
    PyObject *kwds;
} methodcallerobject;

/* from Objects/moduleobject.c */

typedef struct {
    PyObject_HEAD
        PyObject *md_dict;
} PyModuleObject;

#endif /* _PRIVATE_TYPES_H_ */
