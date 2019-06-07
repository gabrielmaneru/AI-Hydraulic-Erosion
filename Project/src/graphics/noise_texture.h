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
	size_t post_resolution{ 1024u };
	int iterations{8};
	float noise_scale{ 7.5f };
	float persistance{0.50f};
	float lacunarity{2.0f};
	float falloff{ 10.0f };
};