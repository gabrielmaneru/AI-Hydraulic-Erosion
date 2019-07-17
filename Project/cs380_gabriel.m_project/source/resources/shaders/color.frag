#version 440
in float c_intensity;
uniform bool doShadow = false;
uniform vec4 base_color;
out vec4 out_color;

void main()
{
	out_color = base_color;
	if(doShadow)
		out_color *=c_intensity;
}