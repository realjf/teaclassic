
#version 330 core

#define SPECULAR_STRENGTH  0.5
#define SPECULAR_SHININESS 2

#define SHADOW_MAP_BIAS 0.002
#define SHADOW_MULTIPLIER 0.7

/*****************************************************************************/
/* INPUTS                                                                    */
/*****************************************************************************/

in VertexToFrag {
         vec2 uv;
    flat int  mat_idx;
         vec3 world_pos;
         vec3 normal;
         vec4 light_space_pos;
    flat int  draw_id;
}from_vertex;

/*****************************************************************************/
/* OUTPUTS                                                                   */
/*****************************************************************************/

out vec4 o_frag_color;

/*****************************************************************************/
/* UNIFORMS                                                                  */
/*****************************************************************************/

uniform vec3 ambient_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 view_pos;

uniform sampler2D shadow_map;

uniform sampler2DArray tex_array0;
uniform sampler2DArray tex_array1;
uniform sampler2DArray tex_array2;
uniform sampler2DArray tex_array3;

/* Per-instance buffer contents:
 *  +--------------------------------------------------+ <-- base
 *  | mat4x4_t (16 floats)                             | (model matrix)
 *  +--------------------------------------------------+
 *  | vec2_t[16] (32 floats)                           | (material:texture mapping)
 *  +--------------------------------------------------+
 *  | {float, float, vec3_t, vec3_t}[16] (128 floats)  | (material properties)
 *  +--------------------------------------------------+
 */

uniform samplerBuffer attrbuff;
uniform int attrbuff_offset;
uniform int attr_stride;

/*****************************************************************************/
/* PROGRAM                                                                   */
/*****************************************************************************/

float shadow_factor(vec4 light_space_pos)
{
    vec3 proj_coords = (light_space_pos.xyz / light_space_pos.w) * 0.5 + 0.5;
    if(proj_coords.x < 0 || proj_coords.x >= textureSize(shadow_map, 0).x)
        return 0.0;
    if(proj_coords.y < 0 || proj_coords.y >= textureSize(shadow_map, 0).y)
        return 0.0;
    if(proj_coords.z > 0.95)
        return 0.0;

    float closest_depth = texture(shadow_map, proj_coords.xy).r;
    float current_depth = proj_coords.z;
    if(current_depth - SHADOW_MAP_BIAS > closest_depth) {
        return 1.0;
    }else {
        return 0.0;
    }
}

int inst_attr_base(int draw_id)
{
    int size = textureSize(attrbuff);
    int inst_offset = draw_id * attr_stride;
    return int(mod(attrbuff_offset / 4 + inst_offset, size));
}

vec3 read_vec3(int base)
{
    int size = textureSize(attrbuff);

    return vec3(
        texelFetch(attrbuff, int(mod(base + 0, size))).r,
        texelFetch(attrbuff, int(mod(base + 1, size))).r,
        texelFetch(attrbuff, int(mod(base + 2, size))).r
    );
}

vec2 read_vec2(int base)
{
    int size = textureSize(attrbuff);

    return vec2(
        texelFetch(attrbuff, int(mod(base + 0, size))).r,
        texelFetch(attrbuff, int(mod(base + 1, size))).r
    );
}

vec4 inst_tex_color(int draw_id, int mat_idx, vec2 uv)
{
    int size = textureSize(attrbuff);
    int table_base = int(mod(inst_attr_base(draw_id) + 16, size));

    vec2 tex_lookup[16];
    for(int i = 0; i < 16; i++) {
        tex_lookup[i] = read_vec2(table_base + i * 2);
    }

    int sampler_idx = int(tex_lookup[mat_idx].x);
    int slice_idx = int(tex_lookup[mat_idx].y);

    switch(sampler_idx) {
    case 0:
        return texture(tex_array0, vec3(uv, slice_idx));
    case 1:
        return texture(tex_array1, vec3(uv, slice_idx));
    case 2:
        return texture(tex_array2, vec3(uv, slice_idx));
    case 3:
        return texture(tex_array3, vec3(uv, slice_idx));
    default:
        return vec4(1.0, 0.0, 1.0, 1.0);
    }
}

void main()
{
    int base = inst_attr_base(from_vertex.draw_id);
    int size = textureSize(attrbuff);

    float ambient_intensity = texelFetch(attrbuff, int(mod(base + 48 + (from_vertex.mat_idx * 8), size))).r;
    vec3 diffuse_clr =  read_vec3(base + 48 + (from_vertex.mat_idx * 8) + 2);
    vec3 specular_clr = read_vec3(base + 48 + (from_vertex.mat_idx * 8) + 5);

    vec4 tex_color = inst_tex_color(from_vertex.draw_id, from_vertex.mat_idx, from_vertex.uv);

    /* Simple alpha test to reject transparent pixels (with mipmapping) */
    tex_color.rgb *= tex_color.a;
    if(tex_color.a <= 0.5)
        discard;

    /* Ambient calculations */
    vec3 ambient = ambient_intensity * ambient_color;

    /* Diffuse calculations */
    vec3 light_dir = normalize(light_pos - from_vertex.world_pos);
    float diff = max(dot(from_vertex.normal, light_dir), 0.0);
    vec3 diffuse = light_color * (diff * diffuse_clr);

    /* Specular calculations */
    vec3 view_dir = normalize(view_pos - from_vertex.world_pos);
    vec3 reflect_dir = reflect(-light_dir, from_vertex.normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), SPECULAR_SHININESS);
    vec3 specular = SPECULAR_STRENGTH * light_color * (spec * specular_clr);

    vec4 final_color = vec4( (ambient + diffuse + specular) * tex_color.xyz, 1.0);
    float shadow = shadow_factor(from_vertex.light_space_pos);
    if(shadow > 0.0) {
        o_frag_color = vec4(final_color.xyz * SHADOW_MULTIPLIER, 1.0);
    }else{
        o_frag_color = final_color;
    }
}

