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
	float persistance{0.25f};
	float lacunarity{4.0f};
};