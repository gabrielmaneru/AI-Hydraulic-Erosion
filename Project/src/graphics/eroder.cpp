#include "eroder.h"
#include "renderer.h"
#include <utils\math_utils.h>
void eroder::initialize(const rasterized_data & data)
{
	if (m_erosion_brushes.size() != data.scale*data.scale)
		m_erosion_brushes.resize(data.scale*data.scale);
	reset();
}

bool eroder::erode(rasterized_data & data, int iterations)
{
	// Iterate the erosion
	if (m_erosion_brushes[0].idx.size() != erosion_radius*erosion_radius)
		create_erosion_brushes(data);
	bool stay = true;
	for (int it = 0; it < iterations; it++)
	{
		create_particles(data);
		if (!iterate(data) && remaining == 0)
		{
			stay = false;
			break;
		}
	}
	data.mesh.compute_terrain_normals();
	data.mesh.load();
	renderer->m_generator.rasterize_texture();
	return stay;
}

void eroder::blur(rasterized_data & data)
{
	if (m_erosion_brushes[0].idx.size() != erosion_radius*erosion_radius)
		create_erosion_brushes(data);
	for (int i = 0; i < m_erosion_brushes.size(); i++)
	{
		float val{ 0.0f };
		for (int j = 0; j < m_erosion_brushes[i].idx.size(); j++)
			val += m_erosion_brushes[i].weight[j] * data.mesh.vertices[m_erosion_brushes[i].idx[j]].y;
		data.mesh.vertices[i].y = lerp(data.mesh.vertices[i].y,val,blur_force);
	}
	data.mesh.compute_terrain_normals();
	data.mesh.load();
	renderer->m_generator.rasterize_texture();
}

void eroder::reset()
{
	m_particles.fill({});
	remaining = 0;
	m_eroding = false;
}

void eroder::create_erosion_brushes(rasterized_data& data)
{
	for (int i = 0; i < m_erosion_brushes.size(); i++)
	{
		auto& brush = m_erosion_brushes[i];
		int centreX = i % data.scale;
		int centreY = i / data.scale;

		float weightSum = 0;
		for (int y = -erosion_radius; y <= erosion_radius; y++)
		{
			for (int x = -erosion_radius; x <= erosion_radius; x++)
			{
				float sqrDst = (float)(x * x + y * y);
				if (sqrDst < erosion_radius * erosion_radius)
				{
					int coordX = centreX + x;
					int coordY = centreY + y;

					if (coordX >= 0 && coordX < data.scale && coordY >= 0 && coordY < data.scale)
					{
						float weight = 1 - sqrtf(sqrDst) / erosion_radius;
						weightSum += weight;
						brush.weight.push_back(weight);
						brush.idx.push_back((y + centreY) * data.scale + x + centreX);
					}
				}
			}
		}
		for (auto& w : brush.weight)
		{
			w /= weightSum;
		}
	}
}

void eroder::create_particles(rasterized_data& data)
{
	for (auto& p : m_particles)
	{
		if (remaining == 0)
			return;
		if(p.active == false)
		{
			p.active = true;
			p.pos = { map(rand(), 0, RAND_MAX, erosion_radius+1.0f, data.scale - erosion_radius - 1.0f),
					  map(rand(), 0, RAND_MAX, erosion_radius+1.0f, data.scale - erosion_radius - 1.0f) };
			p.dir = vec2{ 0.0f };
			p.speed = 1.0f;
			p.water = 1.0f;
			p.sediment = 0;
			p.lifetime = 0;
			remaining--;
		}
	}
}

bool eroder::iterate(rasterized_data& data)
{
	bool still_active = false;
	for (auto& p : m_particles)
	{
		if (p.active)
		{
			// Compute gradient
			vec2 gradient = p.gradient(data);
			// Update direction
			p.dir = p.dir*inertia - gradient * (1 - inertia);
			// Stop if not moving
			if (glm::length2(p.dir) < glm::epsilon<float>())
			{
				p.active = false;
				continue;
			}

			// Store previous height
			float prev_height = p.height(data);
			// Store previous position
			int prev_nodeX = floor_float(p.pos.x);
			int prev_nodeY = floor_float(p.pos.y);
			float prev_deviationX = p.pos.x - prev_nodeX;
			float prev_deviationY = p.pos.y - prev_nodeY;

			// Update position
			p.dir = glm::normalize(p.dir);
			p.pos.x += p.dir.x;
			p.pos.y += p.dir.y;
			// Stop if particle is out of bounds
			if (p.pos.y <= erosion_radius || p.pos.y >= data.scale - erosion_radius || p.pos.x <= erosion_radius || p.pos.x >= data.scale - erosion_radius)
			{
				p.active = false;
				continue;
			}

			// Compute new height
			float new_height = p.height(data);
			float d_height = new_height - prev_height;
			// Compute current capacity
			float sediment_capacity = max(-d_height*p.speed*p.water*capacity_factor, minimum_slope);

			// If more sediment than capacity or flowing upwards
			if (p.sediment > sediment_capacity || d_height > 0.0f)
			{
				if (p.sediment > 0.0f)
				{
					float to_deposit;
					if (d_height > 0)
						to_deposit = min(d_height, p.sediment);
					else
						to_deposit = (p.sediment - sediment_capacity) *deposit_factor;
					p.deposit(data, to_deposit, prev_nodeY * data.scale + prev_nodeX, prev_deviationX, prev_deviationX);
				}
			}
			else
			{
				float to_erode = min((sediment_capacity - p.sediment)*erode_factor, -d_height);
				p.erode(data, to_erode, m_erosion_brushes[prev_nodeY * data.scale + prev_nodeX]);
			}

			if (++p.lifetime == max_lifetime)
			{
				p.active = false;
				continue;
			}
			p.speed = sqrtf(max(p.speed * p.speed + d_height * gravity, 0.0f));
			p.water *= (1 - evaporation);
			still_active = true;
			
		}
	}
	return still_active;
}

vec2 particle::gradient(const rasterized_data & data)
{
	int prev_nodeX = floor_float(pos.x);
	int prev_nodeY = floor_float(pos.y);
	float prev_deviationX = pos.x - prev_nodeX;
	float prev_deviationY = pos.y - prev_nodeY;

	int nodeIndexNW = prev_nodeY * data.scale + prev_nodeX;
	float heightNW = data.mesh.vertices[nodeIndexNW].y;
	float heightNE = data.mesh.vertices[nodeIndexNW + 1].y;
	float heightSW = data.mesh.vertices[nodeIndexNW + data.scale].y;
	float heightSE = data.mesh.vertices[nodeIndexNW + data.scale + 1].y;

	return{ (heightNE - heightNW) * (1 - prev_deviationY)
		+ (heightSE - heightSW) * prev_deviationY,
		(heightSW - heightNW) * (1 - prev_deviationX)
		+ (heightSE - heightNE) * prev_deviationX };
}

float particle::height(const rasterized_data & data)
{
	int prev_nodeX = floor_float(pos.x);
	int prev_nodeY = floor_float(pos.y);
	float prev_deviationX = pos.x - prev_nodeX;
	float prev_deviationY = pos.y - prev_nodeY;

	int nodeIndexNW = prev_nodeY * data.scale + prev_nodeX;
	float heightNW = data.mesh.vertices[nodeIndexNW].y;
	float heightNE = data.mesh.vertices[nodeIndexNW + 1].y;
	float heightSW = data.mesh.vertices[nodeIndexNW + data.scale].y;
	float heightSE = data.mesh.vertices[nodeIndexNW + data.scale + 1].y;

	return heightNW * (1 - prev_deviationX) * (1 - prev_deviationY)
		+ heightNE * prev_deviationX * (1 - prev_deviationY)
		+ heightSW * (1 - prev_deviationX) * prev_deviationY
		+ heightSE * prev_deviationX * prev_deviationY;
}

void particle::deposit(rasterized_data & data, float to_deposit, int dropletIndex, float prev_deviationX, float prev_deviationY)
{
	sediment -= to_deposit;

	data.mesh.vertices[dropletIndex].y += to_deposit * (1 - prev_deviationX)*(1 - prev_deviationY);
	data.mesh.vertices[dropletIndex + 1].y += to_deposit * prev_deviationX*(1 - prev_deviationY);
	data.mesh.vertices[dropletIndex + data.scale].y += to_deposit * (1 - prev_deviationX)*prev_deviationY;
	data.mesh.vertices[dropletIndex + data.scale + 1].y += to_deposit * prev_deviationX*prev_deviationY;
}

void particle::erode(rasterized_data & data, float to_erode, const brush & b)
{
	for (int index = 0; index < b.idx.size(); index++)
	{
		int vtx_index = b.idx[index];
		float amount = to_erode * b.weight[index];

		float d_sediment = min(data.mesh.vertices[vtx_index].y, amount);
		data.mesh.vertices[vtx_index].y -= d_sediment;
		sediment += d_sediment;
	}
}
