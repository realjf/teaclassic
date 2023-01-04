

#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 4) in int  in_draw_id;

/*****************************************************************************/
/* UNIFORMS                                                                  */
/*****************************************************************************/

uniform mat4 light_space_transform;
uniform vec4 clip_plane0;

/* Per-instance buffer contents:
 *  +--------------------------------------------------+ <-- base
 *  | mat4x4_t (16 floats)                             | (model matrix)
 *  +--------------------------------------------------+
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

void main()
{
    int base = inst_attr_base(in_draw_id);
    mat4 model = read_mat4(base);

    gl_Position = light_space_transform * model * vec4(in_pos, 1.0);
    gl_ClipDistance[0] = dot(model * gl_Position, clip_plane0);
}

