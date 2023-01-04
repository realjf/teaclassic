
#version 330 core

#define MAX_MATERIALS 16

/* TODO: Make these as material parameters */
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

struct material{
    float ambient_intensity;
    vec3  diffuse_clr;
    vec3  specular_clr;
};

uniform material materials[MAX_MATERIALS];

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

void main()
{
    vec4 tex_color = texture(tex_array0, vec3(from_vertex.uv, from_vertex.mat_idx));

    /* Simple alpha test to reject transparent pixels (with mipmapping) */
    tex_color.rgb *= tex_color.a;
    if(tex_color.a <= 0.5)
        discard;

    /* Ambient calculations */
    vec3 ambient = materials[from_vertex.mat_idx].ambient_intensity * ambient_color;

    /* Diffuse calculations */
    vec3 light_dir = normalize(light_pos - from_vertex.world_pos);
    float diff = max(dot(from_vertex.normal, light_dir), 0.0);
    vec3 diffuse = light_color * (diff * materials[from_vertex.mat_idx].diffuse_clr);

    /* Specular calculations */
    vec3 view_dir = normalize(view_pos - from_vertex.world_pos);
    vec3 reflect_dir = reflect(-light_dir, from_vertex.normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), SPECULAR_SHININESS);
    vec3 specular = SPECULAR_STRENGTH * light_color * (spec * materials[from_vertex.mat_idx].specular_clr);

    vec4 final_color = vec4( (ambient + diffuse + specular) * tex_color.xyz, 1.0);
    float shadow = shadow_factor(from_vertex.light_space_pos);
    if(shadow > 0.0) {
        o_frag_color = vec4(final_color.xyz * SHADOW_MULTIPLIER, 1.0);
    }else{
        o_frag_color = final_color;
    }
}

