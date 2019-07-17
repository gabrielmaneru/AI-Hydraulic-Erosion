#pragma once
#include "raw_texture.h"
#include "raw_mesh.h"
struct noise_texture
{
	void generate();

	raw_texture_single m_texture;
	raw_mesh m_naive_mesh;

	size_t resolution;
	size_t prev_resolution{ 128u };
#ifdef _DEBUG
	size_t post_resolution{ 128u };
#else
	size_t post_resolution{ 512u };
#endif // _DEBUG
	int iterations{8};
	float noise_scale{ 5.0f };
	float persistance{0.50f};
	float lacunarity{2.0f};
	vec2 falloff{ 0.5f,0.25f };
};