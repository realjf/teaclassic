#ifndef _NAV_PRIVATE_H_
#define _NAV_PRIVATE_H_

#include "public/nav.h"
#include "nav_data.h"
#include "../map/public/tile.h"

#include <stddef.h>

struct portal;

struct nav_private {
    size_t width, height;
    struct nav_chunk *chunks[NAV_LAYER_MAX];
};

enum nav_layer N_DestLayer(dest_id_t id);
int N_DestFactionID(dest_id_t id);

bool N_PortalReachableFromTile(const struct portal *port, struct coord tile,
                               const struct nav_chunk *chunk);

int N_GridNeighbours(const uint8_t cost_field[FIELD_RES_R][FIELD_RES_C], struct coord coord,
                     struct coord out_neighbours[static 8], float out_costs[static 8]);

uint16_t N_ClosestPathableLocalIsland(const struct nav_private *priv, const struct nav_chunk *chunk,
                                      struct tile_desc target);

#endif /* _NAV_PRIVATE_H_ */
