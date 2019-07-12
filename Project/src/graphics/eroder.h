#pragma once
#include "raw_mesh.h"
#include <array>
struct particle
{
	bool active{false};
	float posX;
	float posY;
	vec2  dir;
	float speed;
	float water;
	float sediment;
	int lifetime;
};
constexpr size_t max_particles = 1024;

class eroder
{
public:
	void initialize(const raw_mesh& mesh);
	bool erode(raw_mesh& mesh, int iterations);
	void blur(raw_mesh& mesh);
	void reset();

	// Properties
	float inertia = 0.8f;
	float sediment_factor = 10.0f;
	float minimum_capacity = 0.1f;
	float erode_factor = 0.3f;
	float deposit_factor = 0.3f;
	float evaporate_rate = 0.01f;
	float gravity = 9.8f;
	int max_lifetime = 100;

	// Mode values
	float blur_force{ 0.2f };
	int sbs_count{ 1024 };
	int it_count{ 100000 };
	int it_per_frame{ 1000 };
	int os_count{ 100000 };
	bool m_eroding;
	int remaining;

private:
	void create_erosion_brushes();
	void create_particles();
	bool iterate(raw_mesh& mesh);

	// Properties
	int scale;
	int erosion_radius = 3;

	struct brush
	{
		std::vector<int> idx;
		std::vector<float> weight;
	};
	std::vector<brush> m_erosion_brushes;
	std::array<particle,max_particles> m_particles;
};