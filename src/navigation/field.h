#ifndef _FIELD_H_
#define _FIELD_H_

#include "public/nav.h"
#include "nav_data.h"
#include "../tc_math.h"
#include "../map/public/tile.h"
#include <stdbool.h>

typedef uint64_t ff_id_t;
struct nav_private;

struct LOS_field {
    struct coord chunk;
    struct {
        unsigned int visible : 1;
        unsigned int wavefront_blocked : 1;
    } field[FIELD_RES_R][FIELD_RES_C];
};

struct enemies_desc {
    int faction_id;
    vec3_t map_pos;
    struct coord chunk;
};

struct entity_desc {
    const struct entity *target;
    vec3_t map_pos;
};

struct portal_desc {
    const struct portal *port;
    uint16_t port_iid;
    const struct portal *next;
    uint16_t next_iid;
};

struct field_target {
    enum {
        TARGET_PORTAL,
        TARGET_TILE,
        TARGET_ENEMIES,
        /* Guide to the closest eligible portal. Each set bit represents
         * that the portal at that index is 'eligible'. */
        TARGET_PORTALMASK,
        TARGET_ENTITY,
    } type;
    union {
        struct portal_desc pd;
        struct coord tile;
        struct enemies_desc enemies;
        uint64_t portalmask;
        struct entity_desc ent;
    };
};

struct flow_field {
    struct coord chunk;
    struct field_target target;
    struct {
        unsigned dir_idx : 4;
    } field[FIELD_RES_R][FIELD_RES_C];
};

enum flow_dir {
    FD_NONE = 0,
    FD_NW,
    FD_N,
    FD_NE,
    FD_W,
    FD_E,
    FD_SW,
    FD_S,
    FD_SE
};

extern vec2_t g_flow_dir_lookup[];

/* ------------------------------------------------------------------------
 * Get the unique flow field ID for the specified parameters.
 * ------------------------------------------------------------------------
 */
ff_id_t N_FlowFieldID(struct coord chunk,
                      struct field_target target,
                      enum nav_layer layer);

/* ------------------------------------------------------------------------
 * Extract the navigation layer from the previously generated flow field ID.
 * ------------------------------------------------------------------------
 */
enum nav_layer N_FlowFieldLayer(ff_id_t id);

/* ------------------------------------------------------------------------
 * Extract the field target type from the previously generated flow field ID.
 * ------------------------------------------------------------------------
 */
int N_FlowFieldTargetType(ff_id_t id);

/* ------------------------------------------------------------------------
 * Initialize the field to have a 'FD_NONE' direction at every tile. Regions
 * of the field can then be made to guide towards specifid targets with
 * subsequent calls to the 'Update' family of functions.
 * ------------------------------------------------------------------------
 */
void N_FlowFieldInit(struct coord chunk_coord,
                     const void *nav_private,
                     struct flow_field *out);

/* ------------------------------------------------------------------------
 * Populate the flow field with directions leading units in the field towards
 * the target. If faction_id is not FACTION_ID_NONE, then tiles blocked by
 * enemy factions will not be considered obstacles.
 * ------------------------------------------------------------------------
 */
void N_FlowFieldUpdate(struct coord chunk_coord,
                       const struct nav_private *priv,
                       int faction_id,
                       enum nav_layer layer,
                       struct field_target target,
                       struct flow_field *inout_flow);

/* ------------------------------------------------------------------------
 * Update all tiles with a specific local island ID from the
 * 'local_islands' field for the chunk. The new directions will guide to
 * the closest possible tiles to the original field target. In the case
 * that the original field target tiles all are all on the same local
 * island (local_iid), the field will remain unchanged.
 * ------------------------------------------------------------------------
 */
void N_FlowFieldUpdateIslandToNearest(uint16_t local_iid,
                                      const struct nav_private *priv,
                                      enum nav_layer layer,
                                      int faction_id,
                                      struct flow_field *inout_flow);

/* ------------------------------------------------------------------------
 * Update all tiles for for the 'impassable island' that start is a part of
 * (i.e. the start tile and all impassable tiles that are connected to it via
 * other impassable tiles) to guide to the closest passable tiles on the same
 * chunk. This can be used to guide entities back to a passable tile if they
 * somehow end up on an impassable one.
 * ------------------------------------------------------------------------
 */
void N_FlowFieldUpdateToNearestPathable(const struct nav_chunk *chunk,
                                        struct coord start,
                                        int faction_id,
                                        struct flow_field *inout_flow);

/* ------------------------------------------------------------------------
 * Create a line of sight field, indicating which tiles in this chunk are
 * directly visible from the 'target' tile. If the 'target' tile is not in
 * the current chunk, the previous LOS field along the path must be
 * supplied in the 'prev_los' argument, so that the visibility information
 * can be carried accross the chunk border. This means that the LOS fields
 * must be generated starting at the destination chunk (where 'prev_los' is
 * NULL) and and moving backwards along the path back to the 'source' chunk.
 * ------------------------------------------------------------------------
 */
void N_LOSFieldCreate(dest_id_t id,
                      struct coord chunk_coord,
                      struct tile_desc target,
                      const struct nav_private *priv,
                      vec3_t map_pos,
                      struct LOS_field *out_los,
                      const struct LOS_field *prev_los);

#endif /* _FIELD_H_ */
