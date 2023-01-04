

#version 330 core

#define MAX_JOINTS 96

layout (location = 0) in vec3  in_pos;
layout (location = 4) in ivec3 in_joint_indices0;
layout (location = 5) in ivec3 in_joint_indices1;
layout (location = 6) in vec3  in_joint_weights0;
layout (location = 7) in vec3  in_joint_weights1;
layout (location = 8) in int   in_draw_id;

/*****************************************************************************/
/* UNIFORMS                                                                  */
/*****************************************************************************/

uniform mat4 light_space_transform;
uniform vec4 clip_plane0;

/* The per-instance static attributes have the follwing layout in the buffer:
 *
 *  +--------------------------------------------------+ <-- base
 *  | mat4x4_t (16 floats)                             | (model matrix)
 *  +--------------------------------------------------+
 *  | vec2_t[16] (32 floats)                           | (material:texture mapping)
 *  +--------------------------------------------------+
 *  | {float, float, vec3_t, vec3_t}[16] (128 floats)  | (material properties)
 *  +--------------------------------------------------+
 *  | mat4x4_t (16 floats)                             | (normal matrix)
 *  +--------------------------------------------------+
 *  | MAX_JOINTS * mat4x4_t (1536 floats)              | (curr pose matrices)
 *  +--------------------------------------------------+
 *  | MAX_JOINTS * mat4x4_t (1536 floats)              | (inverse bind pose matrices)
 *  +--------------------------------------------------+
 *
 * In total, 3264 floats (13056 bytes) are pushed per instance.
 */

uniform samplerBuffer attrbuff;
uniform int attrbuff_offset;
uniform int attr_stride;
uniform int attr_offset;

/*****************************************************************************/
/* PROGRAM                                                                   */
/*****************************************************************************/

int inst_attr_base(int draw_id)
{
    int size = textureSize(attrbuff);
    int inst_offset = (attr_offset > 0) ? (attr_offset + gl_InstanceID) * attr_stride
                                        : draw_id * attr_stride;
    return int(mod(attrbuff_offset / 4 + inst_offset, size));
}

vec4 read_vec4(int base)
{
    int size = textureSize(attrbuff);
    return vec4(
        texelFetch(attrbuff, int(mod(base + 0, size))).r,
        texelFetch(attrbuff, int(mod(base + 1, size))).r,
        texelFetch(attrbuff, int(mod(base + 2, size))).r,
        texelFetch(attrbuff, int(mod(base + 3, size))).r
    );
}

mat4 read_mat4(int base)
{
    return mat4(
        read_vec4(base +  0),
        read_vec4(base +  4),
        read_vec4(base +  8),
        read_vec4(base + 12)
    );
}

mat4 anim_curr_pose_mats(int joint_idx)
{
    int base = inst_attr_base(in_draw_id) + 176 + 16;
    return read_mat4(base + (16 * joint_idx));
}

mat4 anim_inv_bind_mats(int joint_idx)
{
    int base = inst_attr_base(in_draw_id) + 176 + 16 + (MAX_JOINTS * 16);
    return read_mat4(base + (16 * joint_idx));
}

void main()
{
    int base = inst_attr_base(in_draw_id);
    mat4 model = read_mat4(base);

    float tot_weight = in_joint_weights0[0] + in_joint_weights0[1] + in_joint_weights0[2]
                     + in_joint_weights1[0] + in_joint_weights1[1] + in_joint_weights1[2];

    /* If all weights are 0, treat this vertex as a static one.
     * Non-animated vertices will have their weights explicitly zeroed out.
     */
    if(tot_weight == 0.0) {

        gl_Position = light_space_transform * model * vec4(in_pos, 1.0);
        gl_ClipDistance[0] = dot(model * gl_Position, clip_plane0);

    }else {

        vec3 new_pos =  vec3(0.0, 0.0, 0.0);
        vec3 new_normal = vec3(0.0, 0.0, 0.0);

        for(int w_idx = 0; w_idx < 6; w_idx++) {

            int joint_idx = int(w_idx < 3 ? in_joint_indices0[w_idx % 3]
                                          : in_joint_indices1[w_idx % 3]);

            mat4 inv_bind_mat = anim_inv_bind_mats (joint_idx);
            mat4 pose_mat     = anim_curr_pose_mats(joint_idx);

            float weight = w_idx < 3 ? in_joint_weights0[w_idx % 3]
                                     : in_joint_weights1[w_idx % 3];
            float fraction = weight / tot_weight;

            mat4 bone_mat = fraction * pose_mat * inv_bind_mat;
            mat3 rot_mat = fraction * mat3(transpose(inverse(pose_mat * inv_bind_mat)));

            new_pos += (bone_mat * vec4(in_pos, 1.0)).xyz;
        }

        gl_Position = light_space_transform * model * vec4(new_pos, 1.0f);
        gl_ClipDistance[0] = dot(model * gl_Position, clip_plane0);
    }
}

