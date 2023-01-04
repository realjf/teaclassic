#ifndef _A_STAR_H_
#define _A_STAR_H_

#include "nav_data.h"
#include "public/nav.h"
#include "../lib/public/vec.h"
#include "../map/public/tile.h"

#include <stdbool.h>

struct nav_private;

/*
 * Say we have the following scenario: there are 3 chunks in a column
 * linked by portals. The middle chunk has some temporary blockers such
 * that it is being divided into 2 islands. We wish to find the list of
 * portals we need to traverse to get from the top to the bottom.
 *
 *   +----------+
 *   |AAAAAAAAAA| (portal A)
 *   +----------+
 *   |0000000000|
 *   |0000000000|
 *   |0000000000| (chunk 1) - [1 local island]
 *   |0000000000|
 *   |0000000000|
 *   +----------+
 *   |BBBBBBBBBB| (portal B)
 *   +----------+
 *   |1111^0000^|
 *   |1111^0000^|
 *   |1111^0000^| (chunk 2) - [2 local islands]
 *   |1111^^^^^^|
 *   |1111111111|
 *   +----------+
 *   |CCCCCCCCCC| (portal C)
 *   +----------+
 *   |0000000000|
 *   |0000000000|
 *   |0000000000| (chunk 3) - [1 local island]
 *   |0000000000|
 *   |0000000000|
 *   +----------+
 *   |DDDDDDDDDD| (portal D)
 *   +----------+
 *   |0000000000|
 *   ............
 *
 * Obviously, this is the list of portals: A -> B -> C -> D
 * However, this information on its' own is not enough. Looking at
 * this example, it can be seen that the blockers are really dividing
 * portal B into two portals - the left side and the right side. If
 * we enter from the right side, we will get trapped in an enclave.
 *
 * As such, we need additional information along with which portals
 * to enter: the local island at which we can enter it on.
 *
 * Using this, the new path now looks like the following series of 'hops':
 * (A, 0) -> (B, 1) -> (C, 0) -> (D, 0)
 */
struct portal_hop {
    const struct portal *portal;
    uint16_t liid;
};

VEC_TYPE(coord, struct coord)
VEC_IMPL(static inline, coord, struct coord)

VEC_TYPE(portal, struct portal_hop)
VEC_IMPL(static inline, portal, struct portal_hop)

/* ------------------------------------------------------------------------
 * Finds the shortest path in a rectangular cost field. Returns true if a
 * path is found, false otherwise. If returning true, 'out_path' holds the
 * tiles to be traversed, in order.
 * ------------------------------------------------------------------------
 */
bool AStar_GridPath(struct coord start, struct coord finish, struct coord chunk,
                    const uint8_t cost_field[FIELD_RES_R][FIELD_RES_C],
                    enum nav_layer layer, vec_coord_t *out_path, float *out_cost);

/* ------------------------------------------------------------------------
 * Finds the shortest path between a tile and a node in a portal graph. Returns
 * true if a path is found, false otherwise. If returning true, 'out_path' holds
 * the portal nodes to be traversed, in order.
 * ------------------------------------------------------------------------
 */
bool AStar_PortalGraphPath(struct tile_desc start_tile, struct tile_desc end_tile,
                           const struct portal *finish, const struct nav_private *priv,
                           enum nav_layer layer, vec_portal_t *out_path, float *out_cost);

#endif /* _A_STAR_H_ */
