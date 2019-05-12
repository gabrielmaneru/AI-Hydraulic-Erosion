#pragma once
#include "map2d.h"
#include "math_utils.h"
#include <glm/glm.h>

map2d<float> generate_noise(size_t width, size_t height, float scale, int iterations, float persistance, float lacunarity, float lowerbound, float upperbound);