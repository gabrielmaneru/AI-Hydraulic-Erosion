#pragma once
#include "raw_texture.h"
#include "raw_mesh.h"
struct noise_texture
{
	void update();

	raw_texture_single m_texture;
	raw_mesh m_mesh;

	size_t resolution{256};
	float scale{1.0f};
	int iterations{4};
	float persistance{0.5f};
	float lacunarity{2.0f};
};