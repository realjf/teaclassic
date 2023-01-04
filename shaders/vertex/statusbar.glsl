

#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;

#define MAX_HBS   (256)

/* Must match the definition in the fragment shader */
#define CURR_HB_HEIGHT  (max(4.0/1080 * curr_res.y, 4.0))
#define CURR_HB_WIDTH   (40.0/1080 * curr_res.y)

/*****************************************************************************/
/* OUTPUTS                                                                   */
/*****************************************************************************/

out VertexToFrag {
         vec2 uv;
        float health_pc;
}to_fragment;

/*****************************************************************************/
/* UNIFORMS                                                                  */
/*****************************************************************************/

/* Should be set up for screenspace rendering */
uniform mat4 view;
uniform mat4 projection;

uniform ivec2 curr_res;

uniform vec2  ent_top_offsets_ss[MAX_HBS];
uniform float ent_health_pc[MAX_HBS];

/*****************************************************************************/
/* PROGRAM
/*****************************************************************************/

void main()
{
    to_fragment.uv = in_uv;
    to_fragment.health_pc = ent_health_pc[gl_InstanceID];

    vec2 ss_pos = vec2(in_pos.x * CURR_HB_WIDTH, in_pos.y * CURR_HB_HEIGHT);
    ss_pos += ent_top_offsets_ss[gl_InstanceID];
    gl_Position = projection * view * vec4(ss_pos, 0.0, 1.0);
}

