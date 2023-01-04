

#version 330 core

layout (location = 0) in vec3 in_pos;

/*****************************************************************************/
/* OUTPUTS                                                                   */
/*****************************************************************************/

out VertexToFrag {
    vec4 clip_space_pos;
    vec3 world_pos;
    vec2 uv;
    vec3 view_dir;
    vec3 light_dir;
}to_fragment;

/*****************************************************************************/
/* UNIFORMS                                                                  */
/*****************************************************************************/

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 view_pos;
uniform vec3 light_pos;

uniform vec2 water_tiling;

/*****************************************************************************/
/* PROGRAM
/*****************************************************************************/

void main()
{
    vec4 ws_pos = model * vec4(in_pos, 1.0);
    vec4 clip_space_pos = projection * view * ws_pos;

    to_fragment.clip_space_pos = clip_space_pos;
    to_fragment.world_pos = ws_pos.xyz;
    to_fragment.uv = vec2(in_pos.x/2.0 + 0.5, in_pos.z/2.0 + 0.5) * water_tiling;
    to_fragment.view_dir = normalize(view_pos - ws_pos.xyz);
    to_fragment.light_dir = normalize(light_pos - ws_pos.xyz);

    gl_Position = clip_space_pos;
}

