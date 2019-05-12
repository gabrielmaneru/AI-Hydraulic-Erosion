#pragma once
#include "raw_texture.h"
#include "raw_mesh.h"
struct noise_texture
{
	void update();

	raw_texture_single m_texture;
	raw_mesh m_mesh;

	size_t resolution{512};
	float scale{3.0f};
	int iterations{8};
	float persistance{0.20f};
	float lacunarity{5.0f};

	float blend_factor{ 0.6f };

	struct level
	{
		vec3 color;
		float height;
	};
	std::vector<level> levels
	{
		{vec3{0.0f, 0.0f, 0.2},  0.25f},
		{vec3{0.2f, 0.2f, 0.8},  0.5f},
		{vec3{0.9f, 0.9f, 0.5},  0.6f},
		{vec3{0.1f, 0.4f, 0.2},  0.9f},
		{vec3{0.4f, 0.4f, 0.1}, 1.0f}
	};
};