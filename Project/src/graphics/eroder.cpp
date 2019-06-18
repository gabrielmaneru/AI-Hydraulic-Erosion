#include "eroder.h"
#include <utils\math_utils.h>
void eroder::initialize(const raw_mesh & mesh)
{
	int count = (int)mesh.vertices.size();
	scale = round_float(sqrtf((float)count));
	m_erosion_brushes.resize(count);

	float weightSum = 0.0f;

	for (int i = 0; i < m_erosion_brushes.size(); i++)
	{
		auto& brush = m_erosion_brushes[i];
		int centreX = i % scale;
		int centreY = i / scale;

		if (centreY > erosion_radius && centreY < scale - erosion_radius && centreX > erosion_radius && centreX < scale - erosion_radius)
		{
			weightSum = 0;
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
		}
		for (auto& w : brush.weight)
		{
			w /= weightSum;
		}
	}
}

void eroder::erode(raw_mesh & mesh, int iterations)
{
	// Iterate the erosion
	for (int i = 0; i < iterations; i++)
	{
		// Create a water droplet
		 posX =  map(rand(), 0, RAND_MAX, 2.0f, (float)(scale - 2));
		 posY =  map(rand(), 0, RAND_MAX, 2.0f, (float)(scale - 2));
		 dir=vec2{ 0.0f };
		 speed = 1.0f;
		 water = 1.0f;
		 sediment = 0;

		auto compute_gradient = [&]()->vec2
		{
			int coordX = floor_float(posX);
			int coordY = floor_float(posY);
			float x = posX - coordX;
			float y = posY - coordY;

			int nodeIndexNW = coordY * scale + coordX;
			float heightNW = mesh.vertices[nodeIndexNW].y;
			float heightNE = mesh.vertices[nodeIndexNW + 1].y;
			float heightSW = mesh.vertices[nodeIndexNW + scale].y;
			float heightSE = mesh.vertices[nodeIndexNW + scale + 1].y;
			return {	(heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y,
						(heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x };
		};
		auto compute_height = [&]()->float
		{
			int coordX = floor_float(posX);
			int coordY = floor_float(posY);
			float x = posX - coordX;
			float y = posY - coordY;

			int nodeIndexNW = coordY * scale + coordX;
			float heightNW = mesh.vertices[nodeIndexNW].y;
			float heightNE = mesh.vertices[nodeIndexNW + 1].y;
			float heightSW = mesh.vertices[nodeIndexNW + scale].y;
			float heightSE = mesh.vertices[nodeIndexNW + scale + 1].y;
			float h = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
			return h * scale/ 2000.f;
		};

		for (int lifetime = 0; lifetime < max_lifetime; lifetime++)
		{
			// Round position
			int nodeX = round_float(posX);
			int nodeY = round_float(posY);
			int dropletIndex = nodeY * scale + nodeX;
			float cellOffestX = posX - nodeX;
			float cellOffsetY = posY - nodeY;

			// Update direction using gradient
			vec2 gradient = compute_gradient();
			dir = dir*inertia - gradient * (1 - inertia);

			// Store previous height
			float prev_height = compute_height();

			// Stop if not moving
			if (glm::length2(dir) < glm::epsilon<float>())
				break;

			// Update position
			dir = glm::normalize(dir);
			posX += dir.x;
			posY += dir.y;
			
			if (posY <= erosion_radius || posY >= scale - erosion_radius || posX <= erosion_radius || posX >= scale - erosion_radius)
				break;

			// Compute new height
			float new_height = compute_height();
			float d_height = new_height - prev_height;

			// Compute current capacity
			float sediment_capacity = glm::max(-d_height*speed*water*sediment_factor, minimum_capacity);

			// If more sediment than capacity or flowing upwards
			if (sediment > sediment_capacity || d_height > 0.0f)
			{
				if (sediment > 0.0f)
				{
					float to_deposit;
					if (d_height > 0)
						to_deposit = glm::min(d_height, sediment);
					else
						to_deposit = (sediment - sediment_capacity) *deposit_factor;

					sediment -= to_deposit;

					mesh.vertices[dropletIndex].y += to_deposit * (1 - cellOffestX)*(1 - cellOffsetY);
					mesh.vertices[dropletIndex + 1].y += to_deposit * cellOffestX*(1 - cellOffsetY);
					mesh.vertices[dropletIndex + scale].y += to_deposit * (1 - cellOffestX)*cellOffsetY;
					mesh.vertices[dropletIndex + scale + 1].y += to_deposit * cellOffestX*cellOffsetY;
				}
			}
			else
			{
				float to_erode = glm::min((sediment_capacity - sediment)*erode_factor, -d_height);

				brush& b = m_erosion_brushes[dropletIndex];
				for (int index = 0; index < b.idx.size(); index++)
				{
					int vtx_index = b.idx[index];
					float amount = to_erode * b.weight[index];

					float d_sediment = glm::min(mesh.vertices[vtx_index].y, amount);
					mesh.vertices[dropletIndex].y -= d_sediment;
					sediment += d_sediment;
				}
			}
			speed = sqrtf(glm::max(speed * speed + d_height * gravity, 0.0f));
			water *= (1 - evaporate_rate);
		}
	}
	mesh.compute_terrain_normals();
	mesh.load();
}