#include "generate_noise.h"

static vec2 random_offset{ 0.0f, 0.0f };
void randomize_noise()
{
	random_offset.x = rand() * rand() / (float)RAND_MAX;
	random_offset.y = rand() * rand() / (float)RAND_MAX ;
}

map2d<float> generate_noise(size_t width, size_t height, float scale, int iterations, float persistance, float lacunarity, float lowerbound, float upperbound)
{
	map2d<float> noise_map{ width, height };

	if (scale <= 0.0f)
		scale = FLT_EPSILON;

	float min_value{ FLT_MAX }, max_value{ -FLT_MAX };
	noise_map.loop(
		[&](size_t x, size_t y, float) -> float
		{
			float amplitude{ 1.0f };
			float frequency{ scale };
			float noise_value{ 0.0f };

			for (int i = 0; i < iterations; ++i)
			{
				float sampler_x = x / (width - 1.0f) * frequency;
				float sampler_y = y / (height - 1.0f) * frequency;

				noise_value += glm::perlin(random_offset + vec2{ sampler_x, sampler_y }) * amplitude;

				amplitude *= persistance;
				frequency *= lacunarity;
			}
			if (noise_value < min_value)
				min_value = noise_value;
			if (noise_value > max_value)
				max_value = noise_value;
			return noise_value;
		}
	);

	noise_map.loop(
		[&](size_t, size_t, float prev) -> float
		{
			return map(prev, min_value, max_value, lowerbound, upperbound);
		}
	);
	return noise_map;
}
