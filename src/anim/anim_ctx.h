#ifndef _ANIM_CTX_H_
#define _ANIM_CTX_H_

#include "public/anim.h"
#include <stddef.h>
#include <stdint.h>

struct anim_ctx {
  const struct anim_clip *active;
  const struct anim_clip *idle;
  const struct anim_data *data;
  enum anim_mode mode;
  unsigned key_fps;
  int curr_frame;
  uint32_t curr_frame_start_ticks;
};

#endif /* _ANIM_CTX_H_ */
