
#version 330 core

#define BORDER_PERCENT_WIDTH (0.05)
#define EPSILON              (1.0 / 1000000.0)

/*****************************************************************************/
/* OUTPUTS                                                                   */
/*****************************************************************************/

out vec4 o_frag_color;

/*****************************************************************************/
/* INPUTS                                                                    */
/*****************************************************************************/

in VertexToFrag {
         vec2 uv;
    flat int  mat_idx;
         vec3 world_pos;
         vec3 normal;
}from_vertex;

/*****************************************************************************/
/* UNIFORMS                                                                  */
/*****************************************************************************/

uniform vec4 color;

/*****************************************************************************/
/* PROGRAM                                                                   */
/*****************************************************************************/

bool inside_border()
{
    if(from_vertex.uv.x < BORDER_PERCENT_WIDTH || from_vertex.uv.x > 1.0 - BORDER_PERCENT_WIDTH)
        return true;

    /* Dont' draw top and bottom edges for side faces */
    if(abs(from_vertex.normal.y) < EPSILON)
        return false;

    if(from_vertex.uv.y < BORDER_PERCENT_WIDTH || from_vertex.uv.y > 1.0 - BORDER_PERCENT_WIDTH)
        return true;

    return false;
}

void main()
{
    if(!inside_border())
        discard;

    o_frag_color = color;
}

