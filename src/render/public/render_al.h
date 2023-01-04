#ifndef _RENDER_AL_H_
#define _RENDER_AL_H_

#include <stdio.h>
#include <SDL_rwops.h>

struct tcobj_hdr;
struct map;
struct tile;

/* ---------------------------------------------------------------------------
 * Consumes lines of the stream and uses them to populate a new private context
 * for the model. The context is returned in a malloc'd buffer.
 * ---------------------------------------------------------------------------
 */
void *R_AL_PrivFromStream(const char *base_path, const struct tcobj_hdr *header, SDL_RWops *stream);

/* ---------------------------------------------------------------------------
 * Dumps private render data in TC Object format.
 * ---------------------------------------------------------------------------
 */
void R_AL_DumpPrivate(FILE *stream, void *priv_data);

/* ---------------------------------------------------------------------------
 * Gives size (in bytes) of buffer size required for the render private
 * buffer for a renderable TCChunk.
 * ---------------------------------------------------------------------------
 */
size_t R_AL_PrivBuffSizeForChunk(size_t tiles_width, size_t tiles_height, size_t num_mats);

/* ---------------------------------------------------------------------------
 * Initialize private render buff for a TCChunk of the map.
 *
 * This function will build the vertices and their vertices from the data
 * already parsed into the 'tiles'.
 * ---------------------------------------------------------------------------
 */
bool R_AL_InitPrivFromTiles(const struct map *map, int chunk_r, int chunk_c,
                            const struct tile *tiles, size_t width, size_t height,
                            void *priv_buff, const char *basedir);

#endif /* _RENDER_AL_H_ */
