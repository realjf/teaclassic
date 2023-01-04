
#version 330 core

precision mediump float;

in VertexToFrag {
    vec2 uv;
    vec4 color;
}from_vertex;

/*****************************************************************************/
/* OUTPUTS                                                                   */
/*****************************************************************************/

out vec4 o_frag_color;

/*****************************************************************************/
/* UNIFORMS                                                                  */
/*****************************************************************************/

uniform sampler2D texture0;

/*****************************************************************************/
/* PROGRAM                                                                   */
/*****************************************************************************/

void main()
{
    o_frag_color = from_vertex.color * texture(texture0, from_vertex.uv);
}

