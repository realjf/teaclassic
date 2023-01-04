
#version 330 core

#define FULL_HP_CLR     vec4(0.0, 1.0, 0.0, 1.0)
#define NO_HP_CLR       vec4(1.0, 0.0, 0.0, 1.0)
#define BG_CLR          vec4(0.0, 0.0, 0.0, 1.0)

#define BORDER_PX_WIDTH (1.0)

/* Must match the definition in the vertex shader */
#define CURR_HB_HEIGHT  (max(4.0/1080 * curr_res.y, 4.0))
#define CURR_HB_WIDTH   (40.0/1080 * curr_res.y)

/*****************************************************************************/
/* INPUTS                                                                    */
/*****************************************************************************/

in VertexToFrag {
         vec2 uv;
        float health_pc;
}from_vertex;

/*****************************************************************************/
/* OUTPUTS                                                                   */
/*****************************************************************************/

out vec4 o_frag_color;

/*****************************************************************************/
/* UNIFORMS                                                                  */
/*****************************************************************************/

uniform ivec2 curr_res;

/*****************************************************************************/
/* PROGRAM                                                                   */
/*****************************************************************************/

void main()
{
    vec4 bar_clr = mix(NO_HP_CLR, FULL_HP_CLR, from_vertex.health_pc);

    /* Slightly darken up the bottom part of the bar */
    if(from_vertex.uv.y > 0.5)
        bar_clr *= 0.8;

    /* We are in the border region */
    if(from_vertex.uv.y < BORDER_PX_WIDTH/CURR_HB_HEIGHT
    || from_vertex.uv.y > (1.0 - BORDER_PX_WIDTH/CURR_HB_HEIGHT)
    || from_vertex.uv.x < BORDER_PX_WIDTH/CURR_HB_WIDTH
    || from_vertex.uv.x > (1.0 - BORDER_PX_WIDTH/CURR_HB_WIDTH))
        o_frag_color = BG_CLR;
    /* We are in the region right of a partially full healthbar */
    else if(from_vertex.uv.x > from_vertex.health_pc)
        o_frag_color = BG_CLR;
    /* We are in the healthbar */
    else
        o_frag_color = bar_clr;
}

