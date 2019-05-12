#version 440
in float vtx_height;
out vec4 out_color;

const float water_limit = 0.5;
const float blend_size = 0.05;
const vec4 water_color = vec4(0.2, 0.2, 0.8, 1.0);
const vec4 grass_color = vec4(0.2, 0.8, 0.2, 1.0);
void main()
{
	if(vtx_height < water_limit - .5*blend_size)
		out_color = water_color;
	else if(vtx_height > water_limit + .5*blend_size)
		out_color = grass_color;
	else
	{
		float factor  = vtx_height - water_limit + .5 * blend_size;
		out_color = water_color + (grass_color - water_color) * factor;
	}
}