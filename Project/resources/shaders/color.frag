#version 440
in float vtx_height;
uniform vec4 base_color;
out vec4 out_color;

void main()
{
	out_color = base_color;
}