#ifndef _TCCHUNK_H_
#define _TCCHUNK_H_

#include "public/tile.h"
#include "public/map.h"
#include "../tc_math.h"

#include <stdbool.h>

struct tcchunk {
    /* ------------------------------------------------------------------------
     * Initialized and used by the rendering subsystem. Holds the mesh data
     * and everything the rendering subsystem needs to render this TCChunk.
     * ------------------------------------------------------------------------
     */
    void *render_private;
    /* ------------------------------------------------------------------------
     * Worldspace position of the top left corner.
     * ------------------------------------------------------------------------
     */
    vec3_t position;
    /* ------------------------------------------------------------------------
     * Each tiles' attributes, stored in row-major order.
     * ------------------------------------------------------------------------
     */
    struct tile tiles[TILES_PER_CHUNK_HEIGHT * TILES_PER_CHUNK_WIDTH];
};

#endif /* _TCCHUNK_H_ */
