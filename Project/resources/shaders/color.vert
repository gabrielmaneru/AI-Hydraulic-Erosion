#version 440
in vec3 attr_pos;
uniform mat4 MVP;

void main()
{
	vec4 vertex = vec4(attr_pos, 1.0f);
	gl_Position = MVP * vertex;
}