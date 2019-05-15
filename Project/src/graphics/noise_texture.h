#pragma once
#include "raw_texture.h"
#include "raw_mesh.h"
struct noise_texture
{
	void update();

	raw_texture_single m_texture;
	raw_mesh m_mesh;

	size_t resolution{256u};
	int iterations{8};
	float noise_scale{ 10.0f };
	float persistance{0.20f};
	float lacunarity{5.0f};
	float falloff{ 1.0f };

	float blend_factor{ 0.75f };
	float display_scale{1000.0f};
	float display_height{ 5.0f };

	struct level
	{
		vec3 color;
		float txt_height;
		float real_height;
	};
	std::vector<level> levels
	{
		{vec3{0.0f, 0.0f, 0.2},  0.25f, 0.0f},
		{vec3{0.2f, 0.2f, 0.8},  0.5f, 0.0f},
		{vec3{0.9f, 0.9f, 0.5},  0.6f, 0.0f},
		{vec3{0.1f, 0.4f, 0.2},  0.9f, 0.6f},
		{vec3{0.4f, 0.4f, 0.1}, 1.0f, 1.0f}
	};
};