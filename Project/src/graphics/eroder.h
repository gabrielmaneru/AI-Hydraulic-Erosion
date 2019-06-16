#pragma once
#include "raw_mesh.h"
struct eroder
{
	void initialize(const raw_mesh& mesh);
	void erode(raw_mesh& mesh, int iterations);

	int scale;
	float inertia = 0.05f;
	float sediment_factor = 4.0f;
	float minimum_capacity = 0.01f;
	float erode_factor = 0.3f;
	float deposit_factor = 0.3f;
	float evaporate_rate = 0.01f;
	float gravity = 4.0f;
	int max_lifetime = 30;
	int erosion_radius = 2;
	struct brush
	{
		std::vector<int> idx;
		std::vector<float> weight;
	};
	std::vector<brush> m_erosion_brushes;
};