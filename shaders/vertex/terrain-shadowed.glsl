

#version 330 core

layout (location = 0) in vec3  in_pos;
layout (location = 1) in vec2  in_uv;
layout (location = 2) in vec3  in_normal;
layout (location = 3) in int   in_material_idx;

layout (location = 4) in int   in_blend_mode;
layout (location = 5) in int   in_mid_indices;
layout (location = 6) in ivec2 in_c1_indices;
layout (location = 7) in ivec2 in_c2_indices;
layout (location = 8) in int   in_tb_indices;
layout (location = 9) in int   in_lr_indices;

/*****************************************************************************/
/* OUTPUTS                                                                   */
/*****************************************************************************/

out VertexToFrag {
         vec2  uv;
    flat int   mat_idx;
         vec3  world_pos;
         vec3  normal;
    flat int   blend_mode;
    flat int   mid_indices;
    flat ivec2 c1_indices;
    flat ivec2 c2_indices;
    flat int   tb_indices;
    flat int   lr_indices;
         vec4  light_space_pos;
}to_fragment;

out VertexToGeo {
    vec3 normal;
}to_geometry;

/*****************************************************************************/
/* UNIFORMS                                                                  */
/*****************************************************************************/

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_space_transform;
uniform vec4 clip_plane0;

/*****************************************************************************/
/* PROGRAM
/*****************************************************************************/

void main()
{
    to_fragment.uv = in_uv;
    to_fragment.mat_idx = in_material_idx;
    to_fragment.world_pos = (model * vec4(in_pos, 1.0)).xyz;
    to_fragment.normal = normalize(mat3(model) * in_normal);
    to_fragment.blend_mode = in_blend_mode;
    to_fragment.mid_indices = in_mid_indices;
    to_fragment.c1_indices = in_c1_indices;
    to_fragment.c2_indices = in_c2_indices;
    to_fragment.tb_indices = in_tb_indices;
    to_fragment.lr_indices = in_lr_indices;
    to_fragment.light_space_pos = light_space_transform * vec4(to_fragment.world_pos, 1.0);

    to_geometry.normal = normalize(mat3(projection * view * model) * in_normal);

    gl_Position = projection * view * model * vec4(in_pos, 1.0);
    gl_ClipDistance[0] = dot(model * vec4(in_pos, 1.0), clip_plane0);
}

