#version 440
in vec3 attr_pos;

const int MAX_LEVELS = 10;
struct LevelData
{
	vec3 color;
	float txt_height;
	float real_height;
};
uniform LevelData levels[MAX_LEVELS];
uniform int active_levels;
uniform mat4 MVP;
uniform float terrain_slope;
out float vtx_height;

int get_level()
{
	int i = 0;
	for(; i < active_levels; i++)
		if(attr_pos.y <= levels[i].txt_height)
			break;
	return i;
}

void main()
{
	vtx_height = attr_pos.y;
	int i = get_level();

	float prev_txt = 0.0;
	float prev_height = 0.0;
	if(i > 0)
	{
		prev_txt = levels[i-1].txt_height;
		prev_height = levels[i-1].real_height;
	}

	float txt_factor = 0.0;
	float txt_size = levels[i].txt_height-prev_txt;
	if(txt_size > 0.0)
		txt_factor = (attr_pos.y-prev_txt) / (levels[i].txt_height-prev_txt);
	
	txt_factor = 1 - pow(1 - txt_factor, terrain_slope);
	
	float real_height = prev_height + (levels[i].real_height - prev_height) * txt_factor;
	gl_Position = MVP * vec4(attr_pos.x, real_height - 0.5, attr_pos.z, 1.0);
}