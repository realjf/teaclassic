#ifndef _ANIM_DATA_H_
#define _ANIM_DATA_H_

#include "public/skeleton.h"
#include "../tc_math.h"
#include "../phys/public/collision.h"

#include <stddef.h>

#define ANIM_NAME_LEN 32

struct anim_sample {
    struct SQT *local_joint_poses;
    struct aabb sample_aabb;
};

struct anim_clip {
    char name[ANIM_NAME_LEN];
    struct skeleton *skel;
    unsigned num_frames;
    struct anim_sample *samples;
};

struct anim_data {
    unsigned num_anims;
    struct skeleton skel;
    struct anim_clip *anims;
};

#endif /* _ANIM_DATA_H_ */
