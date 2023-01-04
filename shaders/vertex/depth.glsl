

#version 330 core

layout (location = 0) in vec3 in_pos;

uniform mat4 model;
uniform mat4 light_space_transform;
uniform vec4 clip_plane0;

void main()
{
    gl_Position = light_space_transform * model * vec4(in_pos, 1.0);
    gl_ClipDistance[0] = dot(model * gl_Position, clip_plane0);
}

