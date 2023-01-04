

#version 330 core

layout (location = 0) in vec2  in_pos;
layout (location = 1) in vec2  in_uv;
layout (location = 2) in vec4  in_color;

/*****************************************************************************/
/* OUTPUTS                                                                   */
/*****************************************************************************/

out VertexToFrag {
    vec2 uv;
    vec4 color;
}to_fragment;

/*****************************************************************************/
/* UNIFORMS                                                                  */
/*****************************************************************************/

uniform mat4 projection;

/*****************************************************************************/
/* PROGRAM                                                                   */
/*****************************************************************************/

void main()
{
    to_fragment.uv = in_uv;
    to_fragment.color = in_color;

    gl_Position = projection * vec4(in_pos.xy, 0, 1);
}

