#version 440
in vec3 attr_pos;
uniform mat4 MVP;
uniform float dt;
out float vtx_height;

void main()
{
	float extra_height = 5*(sin(attr_pos.x+dt) + cos(dt) + sin(5*(dt+attr_pos.z)));
	gl_Position = MVP * vec4(attr_pos.x, attr_pos.y+extra_height, attr_pos.z, 1.0);
}