#version 440
in vec3 attr_pos;
uniform mat4 MVP;
void main()
{
	gl_Position = MVP * vec4(attr_pos, 1.0);
}