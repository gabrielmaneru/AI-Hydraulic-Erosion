#pragma once
#include <vector>
#include <glm/glm.h>
struct raw_mesh
{
	void load();
	void free();
	unsigned int m_vao{ 0 };
	unsigned int m_vtx{ 0 };
	unsigned int m_idx{ 0 };
	unsigned int m_uvs{ 0 };
	
	std::vector<vec3> vertices;
	std::vector<vec2> uv_coord;
	std::vector<unsigned> faces;
};