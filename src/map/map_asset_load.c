
#include "public/map.h"
#include "tcchunk.h"
#include "../asset_load.h"
#include "../render/public/render.h"
#include "../render/public/render_al.h"
#include "../render/public/render_ctrl.h"
#include "../navigation/public/nav.h"
#include "../game/public/game.h"
#include "../lib/public/tc_string.h"
#include "map_private.h"
#include "../ui.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* ASCII to integer - argument must be an ascii digit */
#define A2I(_a) ((_a) - '0')
#define MINIMAP_DFLT_SZ (256)
#define TCMAP_VER       (1.0f)
#define CHK_TRUE(_pred, _label) do{ if(!(_pred)) goto _label; }while(0)

/*****************************************************************************/
/* STATIC FUNCTIONS                                                          */
/*****************************************************************************/

static bool m_al_parse_tile(const char *str, struct tile *out)
{
    if(strlen(str) != 24)
        return false;

    char type_hexstr[2] = {str[0], '\0'};

    memset(out, 0, sizeof(struct tile));
    out->type          = (enum tiletype) strtol(type_hexstr, NULL, 16);
    out->base_height   = (int)           (str[1] == '-' ? -1 : 1) * (10  * A2I(str[2]) + A2I(str[3]));
    out->ramp_height   = (int)           (10  * A2I(str[4]) + A2I(str[5]));
    out->top_mat_idx   = (int)           (100 * A2I(str[6]) + 10 * A2I(str[7 ]) + A2I(str[8 ]));
    out->sides_mat_idx = (int)           (100 * A2I(str[9]) + 10 * A2I(str[10]) + A2I(str[11]));
    out->pathable      = (bool)          A2I(str[12]);
    out->blend_mode    = (int)           A2I(str[13]);
    out->blend_normals = (bool)          A2I(str[14]);

    return true;
}

static bool m_al_write_tile(const struct tile *tile, SDL_RWops *stream)
{
    char buff[MAX_LINE_LEN];
    tc_snprintf(buff, sizeof(buff), "%01X%c%02d%02d%03d%03d%01d%01d%01d000000000",
        tile->type,
        tile->base_height >= 0 ? '+' : '-',
        abs(tile->base_height),
        tile->ramp_height,
        tile->top_mat_idx,
        tile->sides_mat_idx,
        tile->pathable,
        tile->blend_mode,
        tile->blend_normals);

    return SDL_RWwrite(stream, buff, strlen(buff), 1);
}

static bool m_al_read_row(SDL_RWops *stream, struct tile *out, size_t *out_nread)
{
    char line[MAX_LINE_LEN];
    READ_LINE(stream, line, fail);

    char *saveptr;
    /* String points to the first token - the first tile of this row */
    char *string = tc_strtok_r(line, " \t\n", &saveptr);
    assert(out_nread);
    *out_nread = 0;

    while(string) {

        if(!m_al_parse_tile(string, out + *out_nread))
            goto fail;
        (*out_nread)++;
        string = tc_strtok_r(NULL, " \t\n", &saveptr);
    }

    return true;

fail:
    return false;
}

static bool m_al_read_tcchunk(SDL_RWops *stream, struct tcchunk *out)
{
    size_t tiles_read = 0;
    while(tiles_read < TILES_PER_CHUNK_WIDTH * TILES_PER_CHUNK_HEIGHT) {

        size_t tiles_in_row;
        if(!m_al_read_row(stream, out->tiles + tiles_read, &tiles_in_row))
            return false;
        tiles_read += tiles_in_row;
    }

    return true;
}

static bool m_al_read_material(SDL_RWops *stream, char *out_texname)
{
    char line[MAX_LINE_LEN];
    READ_LINE(stream, line, fail);

    char *saveptr;
    char *string = tc_strtok_r(line, " \t\n", &saveptr);

    if(strcmp(string, "material") != 0)
        goto fail;

    string = tc_strtok_r(NULL, " \t\n", &saveptr); /* skip name */
    string = tc_strtok_r(NULL, " \t\n", &saveptr);

    strncpy(out_texname, string, MAX_LINE_LEN);
    return true;

fail:
    return false;
}

static void m_al_patch_adjacency_info(struct map *map)
{
    for(int r = 0; r < map->height; r++) {
    for(int c = 0; c < map->width;  c++) {

        void *chunk_rprivate = map->chunks[r * map->width + c].render_private;
        for(int tile_r = 0; tile_r < TILES_PER_CHUNK_HEIGHT; tile_r++) {
        for(int tile_c = 0; tile_c < TILES_PER_CHUNK_HEIGHT; tile_c++) {

            struct tile_desc desc = (struct tile_desc){r, c, tile_r, tile_c};
            const struct tile *tile = &map->chunks[r * map->width + c].tiles[tile_r * TILES_PER_CHUNK_WIDTH + tile_c];

            R_PushCmd((struct rcmd){
                .func = R_GL_TilePatchVertsBlend,
                .nargs = 3,
                .args = {
                    chunk_rprivate,
                    (void*)G_GetPrevTickMap(),
                    R_PushArg(&desc, sizeof(desc)),
                },
            });

            if(!tile->blend_normals)
                continue;

            R_PushCmd((struct rcmd){
                .func = R_GL_TilePatchVertsSmooth,
                .nargs = 3,
                .args = {
                    chunk_rprivate,
                    (void*)G_GetPrevTickMap(),
                    R_PushArg(&desc, sizeof(desc)),
                },
            });
        }}
    }}
}

static void set_minimap_defaults(struct map *map)
{
    map->minimap_vres = (vec2_t){1920, 1080};
    map->minimap_sz = MINIMAP_DFLT_SZ;
    map->minimap_center_pos = (vec2_t){
        MINIMAP_DFLT_SZ * cos(M_PI/4.0) + 10,
        1080 - (MINIMAP_DFLT_SZ * cos(M_PI/4.0) + 10)};
    map->minimap_resize_mask = ANCHOR_X_LEFT | ANCHOR_Y_BOT;
}

/*****************************************************************************/
/* EXTERN FUNCTIONS                                                          */
/*****************************************************************************/

bool M_AL_InitMapFromStream(const struct tcmap_hdr *header, const char *basedir,
                            SDL_RWops *stream, void *outmap, bool update_navgrid)
{
    struct map *map = outmap;

    map->width = header->num_cols;
    map->height = header->num_rows;
    map->pos = (vec3_t) {0.0f, 0.0f, 0.0f};
    set_minimap_defaults(map);

    /* Read materials */
    char texnames[header->num_materials][256];
    for(int i = 0; i < header->num_materials; i++) {
        if(i >= MAX_NUM_MATS)
            return false;
        if(!m_al_read_material(stream, texnames[i]))
            return false;
        strcpy(map->texnames[i], texnames[i]);
    }
    map->num_mats = header->num_materials;

    struct map_resolution res = (struct map_resolution){
        header->num_cols,
        header->num_rows,
        TILES_PER_CHUNK_WIDTH,
        TILES_PER_CHUNK_HEIGHT
    };

    R_PushCmd((struct rcmd){
        .func = R_GL_MapInit,
        .nargs = 3,
        .args = {
            R_PushArg(texnames, sizeof(texnames)),
            R_PushArg(&header->num_materials, sizeof(header->num_materials)),
            R_PushArg(&res, sizeof(res)),
        },
    });

    /* Read chunks */
    size_t num_chunks = header->num_rows * header->num_cols;
    char *unused_base = (char*)(map + 1);
    unused_base += num_chunks * sizeof(struct tcchunk);

    for(int i = 0; i < num_chunks; i++) {

        if(!m_al_read_tcchunk(stream, map->chunks + i))
            return false;
    }

    for(int i = 0; i < num_chunks; i++) {

        map->chunks[i].render_private = (void*)unused_base;
        size_t renderbuff_sz = R_AL_PrivBuffSizeForChunk(
                               TILES_PER_CHUNK_WIDTH, TILES_PER_CHUNK_HEIGHT, 0);
        unused_base += renderbuff_sz;

        if(!R_AL_InitPrivFromTiles(map, i / header->num_cols, i % header->num_cols,
                                   map->chunks[i].tiles, TILES_PER_CHUNK_WIDTH, TILES_PER_CHUNK_HEIGHT,
                                   map->chunks[i].render_private, basedir)) {
            return false;
        }
    }

    m_al_patch_adjacency_info(map);

    /* Build navigation grid */
    const struct tile *chunk_tiles[map->width * map->height];

    for(int r = 0; r < map->height; r++) {
    for(int c = 0; c < map->width; c++) {
        chunk_tiles[r * map->width + c] = map->chunks[r * map->width + c].tiles;
    }}

    map->nav_private = N_BuildForMapData(map->width, map->height,
        TILES_PER_CHUNK_WIDTH, TILES_PER_CHUNK_HEIGHT, chunk_tiles, update_navgrid);
    if(!map->nav_private)
        return false;

    return true;
}

size_t M_AL_BuffSizeFromHeader(const struct tcmap_hdr *header)
{
    size_t num_chunks = header->num_rows * header->num_cols;

    return sizeof(struct map) + num_chunks *
           (sizeof(struct tcchunk) + R_AL_PrivBuffSizeForChunk(
                                     TILES_PER_CHUNK_WIDTH, TILES_PER_CHUNK_HEIGHT, 0));
}

bool M_AL_UpdateTile(struct map *map, const struct tile_desc *desc, const struct tile *tile)
{
    if(desc->chunk_r >= map->height || desc->chunk_c >= map->width)
        return false;

    struct tcchunk *chunk = &map->chunks[desc->chunk_r * map->width + desc->chunk_c];
    chunk->tiles[desc->tile_r * TILES_PER_CHUNK_WIDTH + desc->tile_c] = *tile;

    struct map_resolution res;
    M_GetResolution(map, &res);

    for(int dr = -1; dr <= 1; dr++) {
    for(int dc = -1; dc <= 1; dc++) {

        struct tile_desc curr = *desc;
        int ret = M_Tile_RelativeDesc(res, &curr, dc, dr);
        if(ret) {

            struct tcchunk *chunk = &map->chunks[curr.chunk_r * map->width + curr.chunk_c];
            R_PushCmd((struct rcmd){
                .func = R_GL_TileUpdate,
                .nargs = 3,
                .args = {
                    chunk->render_private,
                    (void*)G_GetPrevTickMap(),
                    R_PushArg(&curr, sizeof(curr)),
                },
            });
        }
    }}

    return true;
}

void M_AL_FreePrivate(struct map *map)
{
    R_PushCmd((struct rcmd){ .func = R_GL_MapShutdown });
    assert(map->nav_private);
    N_FreePrivate(map->nav_private);
}

size_t M_AL_ShallowCopySize(size_t nrows, size_t ncols)
{
	size_t nchunks = nrows * ncols;
    return sizeof(struct map) + nchunks * sizeof(struct tcchunk);
}

void M_AL_ShallowCopy(struct map *dst, const struct map *src)
{
    memcpy(dst, src, M_AL_ShallowCopySize(src->width, src->height));
}

bool M_AL_WriteTCMap(const struct map *map, SDL_RWops *stream)
{
    char line[MAX_LINE_LEN];

    tc_snprintf(line, sizeof(line), "version %.01f\n", TCMAP_VER);
    CHK_TRUE(SDL_RWwrite(stream, line, strlen(line), 1), fail);

    tc_snprintf(line, sizeof(line), "num_materials %d\n", map->num_mats);
    CHK_TRUE(SDL_RWwrite(stream, line, strlen(line), 1), fail);

    tc_snprintf(line, sizeof(line), "num_rows %d\n", map->height);
    CHK_TRUE(SDL_RWwrite(stream, line, strlen(line), 1), fail);

    tc_snprintf(line, sizeof(line), "num_cols %d\n", map->width);
    CHK_TRUE(SDL_RWwrite(stream, line, strlen(line), 1), fail);

    for(int i = 0; i < map->num_mats; i++) {

        tc_snprintf(line, sizeof(line), "material __anonymous__ %s\n", map->texnames[i]);
        CHK_TRUE(SDL_RWwrite(stream, line, strlen(line), 1), fail);
    }

    for(int chunk_r = 0; chunk_r < map->height; chunk_r++) {
    for(int chunk_c = 0; chunk_c < map->width;  chunk_c++) {

        const struct tcchunk *curr_chunk = &map->chunks[chunk_r * map->width + chunk_c];
        for(int tile_r = 0; tile_r < TILES_PER_CHUNK_HEIGHT; tile_r++) {
        for(int tile_c = 0; tile_c < TILES_PER_CHUNK_WIDTH;  tile_c++) {

            const struct tile *curr_tile = &curr_chunk->tiles[tile_r * TILES_PER_CHUNK_WIDTH + tile_c];
            CHK_TRUE(m_al_write_tile(curr_tile, stream), fail);

            if((tile_c+1) % 4 == 0) {
                CHK_TRUE(SDL_RWwrite(stream, "\n", 1, 1), fail);
            }else {
                CHK_TRUE(SDL_RWwrite(stream, " ", 1, 1), fail);
            }
        }}
    }}

    return true;

fail:
    return false;
}

