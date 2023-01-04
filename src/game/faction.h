#ifndef _FACTION_H_
#define _FACTION_H_

#include "public/game.h"
#include "../tc_math.h"

struct faction {
    vec3_t color;
    char name[MAX_FAC_NAME_LEN];
    bool controllable;
};

#endif /* _FACTION_H_ */
