#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdbool.h>
#include <SDL.h>

#define CONFIG_SCHED_TARGET_FPS (30)
#define CONFIG_USE_BATCH_RENDERING (false)

/* The far end of the camera's clipping frustrum, in OpenGL coordinates */
#define CONFIG_DRAWDIST (1000)
#define CONFIG_TILE_TEX_RES (128)
#define CONFIG_ARR_TEX_RES (512)
#define CONFIG_LOADING_SCREEN "assets/loading_screens/default.png"

#define CONFIG_SHADOW_MAP_RES (2048)
/* Determines the draw distance from the light source when creating the
 * shadow map. Note that a higher drawdistance leads to more peterpanning.
 */
#define CONFIG_SHADOW_DRAWDIST (1536)
/* This is the half-width of the light source's frustum, in OpenGL coordinates.
 * Increasing the FOV results in lower-quality shadows for the same shadow map
 * resolution. However, the light frustum needs to be sufficiently large to
 * contain all shadow casters visible by the RTS camera.
 */
#define CONFIG_SHADOW_FOV (160)

#define CONFIG_SETTINGS_FILENAME "tc.conf"

#define CONFIG_LOS_CACHE_SZ (2048)
#define CONFIG_FLOW_CACHE_SZ (2048)
#define CONFIG_MAPPING_CACHE_SZ (4096)
#define CONFIG_GRID_PATH_CACHE_SZ (8192)

#define CONFIG_FRAME_STEP_HOTKEY (SDL_SCANCODE_SPACE)

/* Some debug configurations to allow overriding malloc/free and friends
 * on Linux builds to assist in debuggin memory problems. See debug_malloc.c
 * for details.
 */
#define CONFIG_USE_DEBUG_ALLOCATOR (false)
#define CONFIG_DEBUG_ALLOC_MMAP (false)

#endif /* _CONFIG_H_ */
