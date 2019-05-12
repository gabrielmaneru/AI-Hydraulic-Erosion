#version 440
in vec3 attr_pos;
uniform mat4 MVP;
out float vtx_height;

void main()
{
	vec4 vertex = vec4(attr_pos, 1.0f);
	vtx_height = attr_pos.y;
	gl_Position = MVP * vertex;
}