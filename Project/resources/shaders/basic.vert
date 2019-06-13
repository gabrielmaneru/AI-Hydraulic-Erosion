#version 440
in vec3 attr_pos;
in vec2 attr_uvs;
in vec3 attr_norm;

uniform mat4 MVP;
uniform float z_off = 0.0f;

out vec2 uv;
out vec3 normal;
void main()
{
	uv = attr_uvs;
	normal = attr_norm;

	gl_Position = MVP * vec4(attr_pos, 1.0);
	gl_Position.z -= z_off;
}