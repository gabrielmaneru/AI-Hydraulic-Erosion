#pragma once
#include <stdint.h>
struct s_quad
{
	uint32_t vao{ 0 };
	uint32_t cnt{};
	void load();
};
extern s_quad quad;