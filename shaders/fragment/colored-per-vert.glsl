
#version 330 core

out vec4 o_frag_color;

in VertexToFrag {
         vec4 color;
}from_vertex;

void main()
{
    o_frag_color = from_vertex.color;
}

