#pragma once
#include "raw_mesh.h"
struct eroder
{
	void initialize(const raw_mesh& mesh);
	void erode(raw_mesh& mesh, int iterations);

	// Properties
	int scale;
	float inertia = 0.2f;
	float sediment_factor = 10.0f;
	float minimum_capacity = 0.1f;
	float erode_factor = 0.3f;
	float deposit_factor = 0.3f;
	float evaporate_rate = 0.01f;
	float gravity = 9.8f;
	int max_lifetime = 100;
	int erosion_radius = 2;

	// Stats
	// Create a water droplet
	float posX;
	float posY;
	vec2  dir;
	float speed;
	float water;
	float sediment;

	struct brush
	{
		std::vector<int> idx;
		std::vector<float> weight;
	};
	std::vector<brush> m_erosion_brushes;
};