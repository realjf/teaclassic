
#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;
layout (location = 2) in vec2 in_offset;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VertexToFrag {
         vec4 color;
}to_fragment;

void main()
{
    to_fragment.color = vec4(in_color, 1.0);
    gl_Position = projection * view * model * vec4(in_pos + vec3(in_offset, 0.0), 1.0);
}

