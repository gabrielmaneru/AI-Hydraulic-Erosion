#pragma once
#include <vector>
#include <glm/glm.h>
struct raw_mesh
{
	void load();
	unsigned int m_vao{ 0 };
	unsigned int m_vtx{ 0 };
	unsigned int m_idx{ 0 };
	
	std::vector<vec3> vertices;
	std::vector<unsigned> faces;
};