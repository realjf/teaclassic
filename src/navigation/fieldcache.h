#ifndef _FIELDCACHE_H_
#define _FIELDCACHE_H_

#include "public/nav.h"
#include "nav_data.h"
#include "field.h"
#include "a_star.h"

#include <stdbool.h>

/*###########################################################################*/
/* FC GENERAL                                                                */
/*###########################################################################*/

bool N_FC_Init(void);
void N_FC_Shutdown(void);

/* Invalidate all LOS and Flow fields for a particular chunk
 */
void N_FC_InvalidateAllAtChunk(struct coord chunk, enum nav_layer layer);

/* Invalidate all LOS and Flow fields for paths (identified by the dest_id) which
 * have at least one field at the specified chunk
 */
void N_FC_InvalidateAllThroughChunk(struct coord chunk, enum nav_layer layer);

/* Invalidate 'enemy seek' fields in all chunks which are adjacent to the
 * current one. This is because 'enemy seek' fields are also dependent
 * on the state of the units in adjacent chunks.
 */
void N_FC_InvalidateNeighbourEnemySeekFields(int width, int height,
                                             struct coord chunk, enum nav_layer layer);

void N_FC_InvalidateDynamicSurroundFields(void);

/*###########################################################################*/
/* LOS FIELD CACHING                                                         */
/*###########################################################################*/

/* Returned pointer should not be cached, as it may become invalid after eviction.
 */
const struct LOS_field *N_FC_LOSFieldAt(dest_id_t id, struct coord chunk_coord);

bool N_FC_ContainsLOSField(dest_id_t id, struct coord chunk_coord);
void N_FC_PutLOSField(dest_id_t id, struct coord chunk_coord,
                      const struct LOS_field *lf);

/*###########################################################################*/
/* FLOW FIELD CACHING                                                        */
/*###########################################################################*/

/* Returned pointer should not be cached, as it may become invalid after eviction.
 */
const struct flow_field *N_FC_FlowFieldAt(ff_id_t ffid);

bool N_FC_ContainsFlowField(ff_id_t ffid);
void N_FC_PutFlowField(ff_id_t ffid, const struct flow_field *ff);

bool N_FC_GetDestFFMapping(dest_id_t id, struct coord chunk_coord,
                           ff_id_t *out_ff);
void N_FC_PutDestFFMapping(dest_id_t dest_id, struct coord chunk_coord,
                           ff_id_t ffid);

/*###########################################################################*/
/* GRID PATH CACHING                                                         */
/*###########################################################################*/

struct grid_path_desc {
    bool exists;
    vec_coord_t path;
    float cost;
};

bool N_FC_GetGridPath(struct coord local_start, struct coord local_dest,
                      struct coord chunk, enum nav_layer layer, struct grid_path_desc *out);
void N_FC_PutGridPath(struct coord local_start, struct coord local_dest,
                      struct coord chunk, enum nav_layer layer, const struct grid_path_desc *in);

#endif /* _FIELDCACHE_H_ */
