

#version 330 core

#define MAX_JOINTS 96
#define USE_GEOMETRY 0

layout (location = 0) in vec3  in_pos;
layout (location = 1) in vec2  in_uv;
layout (location = 2) in vec3  in_normal;
layout (location = 3) in int   in_material_idx;
layout (location = 4) in ivec3 in_joint_indices0;
layout (location = 5) in ivec3 in_joint_indices1;
layout (location = 6) in vec3  in_joint_weights0;
layout (location = 7) in vec3  in_joint_weights1;

/*****************************************************************************/
/* OUTPUTS                                                                   */
/*****************************************************************************/

out VertexToFrag {
         vec2 uv;
    flat int  mat_idx;
         vec3 world_pos;
         vec3 normal;
         vec4 light_space_pos;
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

uniform mat4 anim_curr_pose_mats[MAX_JOINTS];
uniform mat4 anim_inv_bind_mats [MAX_JOINTS];
uniform mat4 anim_normal_mat;

/*****************************************************************************/
/* PROGRAM
/*****************************************************************************/

void main()
{
    to_fragment.uv = in_uv;
    to_fragment.mat_idx = in_material_idx;

#if USE_GEOMETRY
    mat3 normal_matrix_geo = mat3(transpose(inverse(view * model)));
#endif
    mat3 normal_matrix = mat3(anim_normal_mat);

    float tot_weight = in_joint_weights0[0] + in_joint_weights0[1] + in_joint_weights0[2]
                     + in_joint_weights1[0] + in_joint_weights1[1] + in_joint_weights1[2];

    /* If all weights are 0, treat this vertex as a static one.
     * Non-animated vertices will have their weights explicitly zeroed out.
     */
    if(tot_weight == 0.0) {

#if USE_GEOMETRY
        to_geometry.normal = normalize(vec3(projection * vec4(normal_matrix_geo * in_normal, 1.0)));
#endif

        to_fragment.normal = normalize(normal_matrix * in_normal);
        to_fragment.world_pos = (model * vec4(in_pos, 1.0)).xyz;
        to_fragment.light_space_pos = light_space_transform * vec4(to_fragment.world_pos, 1.0);

        gl_Position = projection * view * model * vec4(in_pos, 1.0);
        gl_ClipDistance[0] = dot(model * vec4(in_pos, 1.0), clip_plane0);

    }else {

        vec3 new_pos =  vec3(0.0, 0.0, 0.0);
        vec3 new_normal = vec3(0.0, 0.0, 0.0);

        for(int w_idx = 0; w_idx < 6; w_idx++) {

            int joint_idx = int(w_idx < 3 ? in_joint_indices0[w_idx % 3]
                                          : in_joint_indices1[w_idx % 3]);

            mat4 inv_bind_mat = anim_inv_bind_mats [joint_idx];
            mat4 pose_mat     = anim_curr_pose_mats[joint_idx];

            float weight = w_idx < 3 ? in_joint_weights0[w_idx % 3]
                                     : in_joint_weights1[w_idx % 3];
            float fraction = weight / tot_weight;

            mat4 bone_mat = fraction * pose_mat * inv_bind_mat;
            mat3 rot_mat = fraction * mat3(transpose(inverse(pose_mat * inv_bind_mat)));

            new_pos += (bone_mat * vec4(in_pos, 1.0)).xyz;
            new_normal += rot_mat * in_normal;
        }

#if USE_GEOMETRY
        to_geometry.normal = normalize(normal_matrix_geo * new_normal);
#endif

        to_fragment.normal = normalize(normal_matrix * new_normal);
        to_fragment.world_pos = (model * vec4(new_pos, 1.0)).xyz;
        to_fragment.light_space_pos = light_space_transform * vec4(to_fragment.world_pos, 1.0);

        gl_Position = projection * view * model * vec4(new_pos, 1.0f);
        gl_ClipDistance[0] = dot(model * vec4(in_pos, 1.0), clip_plane0);
    }
}

