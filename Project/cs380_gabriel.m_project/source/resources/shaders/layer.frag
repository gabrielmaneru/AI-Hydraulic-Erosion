#version 440
in float vtx_height;

const int MAX_LEVELS = 10;
struct LevelData
{
	vec3 color;
	float txt_height;
	float real_height;
};
uniform LevelData levels[MAX_LEVELS];
uniform int active_levels;
uniform float blend_factor;

uniform vec4 base_color;
uniform bool useColor;

out vec4 out_color;

void main()
{
	if(useColor)
	{
		out_color = base_color;
		return;
	}
	vec3 level_color;
	for(int i = 0; i < active_levels; i++)
	{
		if(vtx_height <= levels[i].txt_height)
		{
			level_color = levels[i].color;

			float prev = 0.0,post = 1.0;
			if(i > 0)
				prev = levels[i-1].txt_height;

			if( i < active_levels - 1)
				post = levels[i].txt_height;

			float factor = (vtx_height-prev) / (post-prev);
			if(factor < blend_factor && i > 0)
			{
				level_color += (levels[i-1].color - level_color) * ((blend_factor - factor)/(blend_factor*2));
			}
			if((1-factor) < blend_factor && (i < active_levels - 1))
			{
				level_color += (levels[i+1].color - level_color) * ((blend_factor - (1-factor))/(blend_factor*2));
			}
			out_color = vec4(level_color, 1.0);
			return;
		}
	}
}