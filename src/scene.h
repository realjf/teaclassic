#ifndef _SCENE_H_
#define _SCENE_H_

#include "tc_math.h"
#include "lib/public/attr.h"
#include "lib/public/vec.h"
#include "lib/public/khash.h"

#include <stdbool.h>

KHASH_DECLARE(attr, kh_cstr_t, struct attr)

VEC_TYPE(attr, struct attr)
VEC_PROTOTYPES(extern, attr, struct attr)

bool Scene_Load(const char *path);

#endif /* _SCENE_H_ */
