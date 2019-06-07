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
uniform float z_off = 0.0;
out float vtx_height;

int get_level()
{
	int i = 0;
	for(; i < active_levels; i++)
		if(attr_pos.y <= levels[i].txt_height)
			break;
	return i;
}

float get_prev_height(int level)
{
	float acc = 0;
	for(int i = 0; i < level; i++)
		acc +=levels[i].real_height;
	return acc;
}

void main()
{
	vtx_height = attr_pos.y;

	// Get Current level
	int i = get_level();

	// Get Accumulated Height
	float prev_height = get_prev_height(i);

	// Get base level
	float prev_level = 0.0;
	if(i > 0)
		prev_level = levels[i-1].txt_height;

	// Get current level ratio
	float level_ratio = 0.0;
	float level_size = levels[i].txt_height-prev_level;
	if(level_size > 0.0)
		level_ratio = (attr_pos.y-prev_level) / level_size;
	
	// Curve it
	level_ratio = 1 - pow(1 - level_ratio, terrain_slope);
	
	// Compute real height
	float real_height = prev_height + levels[i].real_height * level_ratio;
	gl_Position = MVP * vec4(attr_pos.x, real_height, attr_pos.z, 1.0);
	gl_Position.z -= z_off;
}