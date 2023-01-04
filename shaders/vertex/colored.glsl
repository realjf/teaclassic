

#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec4 in_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VertexToFrag {
         vec4 color;
}to_fragment;

void main()
{
    to_fragment.color = in_color;
    gl_Position = projection * view * model * vec4(in_pos, 1.0);
}

