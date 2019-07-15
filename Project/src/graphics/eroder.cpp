#include "eroder.h"
#include <utils\math_utils.h>
void eroder::initialize(const raw_mesh & mesh)
{
	int count = (int)mesh.vertices.size();
	scale = round_float(sqrtf((float)count));
	if (m_erosion_brushes.size() != count)
		m_erosion_brushes.resize(count);
	reset();
}

bool eroder::erode(raw_mesh & mesh, int iterations)
{
	// Iterate the erosion
	if (m_erosion_brushes[0].idx.size() != erosion_radius*erosion_radius)
		create_erosion_brushes();
	for (int it = 0; it < iterations; it++)
	{
		create_particles();
		if (!iterate(mesh) && remaining == 0)
		{
			mesh.compute_terrain_normals();
			mesh.load();
			return false;
		}
	}
	mesh.compute_terrain_normals();
	mesh.load();
	
	return true;
}

void eroder::blur(raw_mesh & mesh)
{
	if (m_erosion_brushes[0].idx.size() != erosion_radius*erosion_radius)
		create_erosion_brushes();
	for (int i = 0; i < m_erosion_brushes.size(); i++)
	{
		float val{ 0.0f };
		for (int j = 0; j < m_erosion_brushes[i].idx.size(); j++)
			val += m_erosion_brushes[i].weight[j] * mesh.vertices[m_erosion_brushes[i].idx[j]].y;
		mesh.vertices[i].y = lerp(mesh.vertices[i].y,val,blur_force);
	}
	mesh.compute_terrain_normals();
	mesh.load();
}

void eroder::reset()
{
	m_particles.fill({});
	remaining = 0;
	m_eroding = false;
}

void eroder::create_erosion_brushes()
{
	for (int i = 0; i < m_erosion_brushes.size(); i++)
	{
		auto& brush = m_erosion_brushes[i];
		int centreX = i % scale;
		int centreY = i / scale;

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

					if (coordX >= 0 && coordX < scale && coordY >= 0 && coordY < scale)
					{
						float weight = 1 - sqrtf(sqrDst) / erosion_radius;
						weightSum += weight;
						brush.weight.push_back(weight);
						brush.idx.push_back((y + centreY) * scale + x + centreX);
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

void eroder::create_particles()
{
	for (auto& p : m_particles)
	{
		if (remaining == 0)
			return;
		if(p.active == false)
		{
			p.active = true;
			p.posX = map(rand(), 0, RAND_MAX, 2.0f, (float)(scale - 2));
			p.posY = map(rand(), 0, RAND_MAX, 2.0f, (float)(scale - 2));
			p.dir = vec2{ 0.0f };
			p.speed = 1.0f;
			p.water = 1.0f;
			p.sediment = 0;
			p.lifetime = 0;
			remaining--;
		}
	}
}

bool eroder::iterate(raw_mesh& mesh)
{
	auto compute_gradient = [&](const particle&p)->vec2
	{
		int coordX = floor_float(p.posX);
		int coordY = floor_float(p.posY);
		float x = p.posX - coordX;
		float y = p.posY - coordY;

		int nodeIndexNW = coordY * scale + coordX;
		float heightNW = mesh.vertices[nodeIndexNW].y;
		float heightNE = mesh.vertices[nodeIndexNW + 1].y;
		float heightSW = mesh.vertices[nodeIndexNW + scale].y;
		float heightSE = mesh.vertices[nodeIndexNW + scale + 1].y;
		return{ (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y,
			(heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x };
	};
	auto compute_height = [&](const particle&p)->float
	{
		int coordX = floor_float(p.posX);
		int coordY = floor_float(p.posY);
		float x = p.posX - coordX;
		float y = p.posY - coordY;

		int nodeIndexNW = coordY * scale + coordX;
		float heightNW = mesh.vertices[nodeIndexNW].y;
		float heightNE = mesh.vertices[nodeIndexNW + 1].y;
		float heightSW = mesh.vertices[nodeIndexNW + scale].y;
		float heightSE = mesh.vertices[nodeIndexNW + scale + 1].y;
		float h = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
		return h * scale / 2000.f;
	};
	bool still_active = false;
	for (auto& p : m_particles)
	{
		if (p.active)
		{
			// Round position
			int nodeX = round_float(p.posX);
			int nodeY = round_float(p.posY);
			int dropletIndex = nodeY * scale + nodeX;
			float cellOffestX = p.posX - nodeX;
			float cellOffsetY = p.posY - nodeY;

			// Update direction using gradient
			vec2 gradient = compute_gradient(p);
			p.dir = p.dir*inertia - gradient * (1 - inertia);

			// Store previous height
			float prev_height = compute_height(p);

			// Stop if not moving
			if (glm::length2(p.dir) < glm::epsilon<float>())
			{
				p.active = false;
				continue;
			}

			// Update position
			p.dir = glm::normalize(p.dir);
			p.posX += p.dir.x;
			p.posY += p.dir.y;

			if (p.posY <= erosion_radius || p.posY >= scale - erosion_radius || p.posX <= erosion_radius || p.posX >= scale - erosion_radius)
			{
				p.active = false;
				continue;
			}

			// Compute new height
			float new_height = compute_height(p);
			float d_height = new_height - prev_height;

			// Compute current capacity
			float sediment_capacity = glm::max(-d_height*p.speed*p.water*capacity_factor, minimum_capacity);

			// If more sediment than capacity or flowing upwards
			if (p.sediment > sediment_capacity || d_height > 0.0f)
			{
				if (p.sediment > 0.0f)
				{
					float to_deposit;
					if (d_height > 0)
						to_deposit = glm::min(d_height, p.sediment);
					else
						to_deposit = (p.sediment - sediment_capacity) *deposit_factor;

					p.sediment -= to_deposit;

					mesh.vertices[dropletIndex].y += to_deposit * (1 - cellOffestX)*(1 - cellOffsetY);
					mesh.vertices[dropletIndex + 1].y += to_deposit * cellOffestX*(1 - cellOffsetY);
					mesh.vertices[dropletIndex + scale].y += to_deposit * (1 - cellOffestX)*cellOffsetY;
					mesh.vertices[dropletIndex + scale + 1].y += to_deposit * cellOffestX*cellOffsetY;
				}
			}
			else
			{
				float to_erode = glm::min((sediment_capacity - p.sediment)*erode_factor, -d_height);

				brush& b = m_erosion_brushes[dropletIndex];
				for (int index = 0; index < b.idx.size(); index++)
				{
					int vtx_index = b.idx[index];
					float amount = to_erode * b.weight[index];

					float d_sediment = glm::min(mesh.vertices[vtx_index].y, amount);
					mesh.vertices[dropletIndex].y -= d_sediment;
					p.sediment += d_sediment;
				}
			}
			if (++p.lifetime == max_lifetime)
			{
				p.active = false;
				continue;
			}
			p.speed = sqrtf(glm::max(p.speed * p.speed + d_height * gravity, 0.0f));
			p.water *= (1 - evaporate_rate);
			still_active = true;
			
		}
	}
	return still_active;
}
