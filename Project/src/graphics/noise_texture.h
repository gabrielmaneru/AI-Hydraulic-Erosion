#pragma once
#include "raw_texture.h"
#include "raw_mesh.h"
struct noise_texture
{
	void update();

	raw_texture_single m_texture;
	raw_mesh m_mesh;
	raw_mesh m_water;

	size_t resolution{512u};
	int iterations{8};
	float noise_scale{ 7.5f };
	float persistance{0.50f};
	float lacunarity{2.0f};
	float falloff{ 4.0f };

	float blend_factor{ 0.75f };
	float terrain_slope{ 1.5f };
	float display_scale{2000.0f};
	float display_height{ 2.0f };

	struct level
	{
		vec3 color;
		float txt_height;
		float real_height;
	};
	std::vector<level> levels
	{
		{vec3{0.941f, 0.941f, 0.471f}, 0.4f, 15.f},
		{vec3{0.118f, 0.471f, 0.235f}, 0.7f, 20.f},
		{vec3{0.275f, 0.234f, 0.078f}, 0.9f, 20.f},
		{vec3{0.196f, 0.196f, 0.196f}, 1.0f, 10.f}
	};
};