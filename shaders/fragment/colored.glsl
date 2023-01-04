
#version 330 core

out vec4 o_frag_color;

uniform vec4 color;

void main()
{
    o_frag_color = color;
}

