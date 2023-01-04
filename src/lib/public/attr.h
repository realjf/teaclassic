#ifndef _ATTR_H_
#define _ATTR_H_

#include "../../tc_math.h"
#include <stdbool.h>

struct SDL_RWops;

struct attr {
    char key[64];
    enum {
        TYPE_STRING,
        TYPE_FLOAT,
        TYPE_INT,
        TYPE_VEC2,
        TYPE_VEC3,
        TYPE_QUAT,
        TYPE_BOOL,
    } type;
    union {
        char as_string[256];
        float as_float;
        int as_int;
        vec2_t as_vec2;
        vec3_t as_vec3;
        quat_t as_quat;
        bool as_bool;
    } val;
};

/* 'named' attributes start with a single token for the name */
bool Attr_Parse(struct SDL_RWops *stream, struct attr *out, bool named);
bool Attr_Write(struct SDL_RWops *stream, const struct attr *in, const char name[static 0]);

#endif /* _ATTR_H_ */
