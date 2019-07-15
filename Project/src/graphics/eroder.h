#pragma once
#include "raw_mesh.h"
#include "raw_texture.h"
#include <array>

struct rasterized_data
{
	raw_mesh mesh;
	raw_texture_rgb texture;
	int scale;
};

struct brush
{
	std::vector<int> idx;
	std::vector<float> weight;
};

struct particle
{
	bool active{false};
	vec2 pos;
	vec2  dir;
	float speed;
	float water;
	float sediment;
	int lifetime;
	vec3 color;

	vec2 gradient(const rasterized_data& data);
	float height(const rasterized_data& data);

	void deposit(rasterized_data& data, float to_deposit, int dropletIndex, float prev_deviationX, float prev_deviationY);
	void erode(rasterized_data& data, float to_erode, const brush& b);
};
constexpr size_t max_particles = 1024;

class eroder
{
public:
	void initialize(const rasterized_data& data);
	bool erode(rasterized_data& data, int iterations);
	void blur(rasterized_data& data);
	void reset();

	// Properties
	float inertia = 0.05f;
	float capacity_factor = 4.00f;
	float minimum_slope = 0.001f;
	float erode_factor = 0.1f;
	float deposit_factor = 0.1f;
	float evaporation = 0.05f;
	float gravity = 9.8f;
	int max_lifetime = 100;
	int erosion_radius = 3;

	// Mode values
	float blur_force{ 0.2f };
	int sbs_count{ 1024 };
	int it_count{ 100000 };
	int it_per_frame{ 1000 };
	int os_count{ 500000 };
	bool m_eroding;
	int remaining;

private:
	void create_erosion_brushes(rasterized_data& data);
	void create_particles(rasterized_data& data);
	bool iterate(rasterized_data& data);

	std::vector<brush> m_erosion_brushes;
	std::array<particle,max_particles> m_particles;
};