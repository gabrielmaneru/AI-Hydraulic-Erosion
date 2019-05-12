#version 440
in vec3 attr_pos;
uniform mat4 MVP;
out float vtx_height;


void main()
{
	vtx_height = attr_pos.y;
	gl_Position = MVP * vec4(attr_pos.x, attr_pos.y - 0.5, attr_pos.z, 1.0);
}