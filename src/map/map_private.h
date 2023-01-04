#ifndef _MAP_PRIVATE_H_
#define _MAP_PRIVATE_H_

#include "tcchunk.h"
#include "../tc_math.h"

#define MAX_NUM_MATS (256)


struct map{
    /* ------------------------------------------------------------------------
     * Map dimensions in numbers of chunks.
     * ------------------------------------------------------------------------
     */
    size_t width, height;
    /* ------------------------------------------------------------------------
     * World-space location of the top left corner of the map.
     * ------------------------------------------------------------------------
     */
    vec3_t pos;
    /* ------------------------------------------------------------------------
     * Virtual resolution used to draw the minimap.Other parameters
     * assume that this is the screen resolution. The minimap is then scaled as
     * necessary for the current window resolution at the rendering stage.
     * ------------------------------------------------------------------------
     */
    vec2_t minimap_vres;
    /* ------------------------------------------------------------------------
     * Minimap center location, in virtual screen coordinates.
     * ------------------------------------------------------------------------
     */
    vec2_t minimap_center_pos;
    /* ------------------------------------------------------------------------
     * Minimap side length, in virtual screen coordinates.
     * ------------------------------------------------------------------------
     */
    int minimap_sz;
    /* ------------------------------------------------------------------------
     * Controls the minimap bounds as the screen aspect ratio changes. (see ui.h)
     * ------------------------------------------------------------------------
     */
    int minimap_resize_mask;
    /* ------------------------------------------------------------------------
     * Navigation private data for the map.
     * ------------------------------------------------------------------------
     */
    void *nav_private;
    /* ------------------------------------------------------------------------
     * Save the materials information read from the source TCMap file. This is
     * used when saving to a new TCMap file.
     * ------------------------------------------------------------------------
     */
    size_t num_mats;
    char texnames[MAX_NUM_MATS][256];
    /* ------------------------------------------------------------------------
     * The map chunks stored in row-major order. In total, there must be
     * (width * height) number of chunks.
     * ------------------------------------------------------------------------
     */
    struct tcchunk chunks[];
};

struct chunkpos{
    int r, c;
};

void M_ModelMatrixForChunk(const struct map *map, struct chunkpos p, mat4x4_t *out);


#endif /* _MAP_PRIVATE_H_ */
