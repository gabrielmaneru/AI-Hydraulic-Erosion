#include "noise_texture.h"
#include <utils/generate_noise.h>
void noise_texture::update()
{
	map2d<float> new_texture = generate_noise(resolution, scale, iterations, persistance, lacunarity, 0.0f, 1.0f, falloff);
	m_texture.m_values = new_texture.m_values;
	m_texture.m_height = new_texture.m_height;
	m_texture.m_width = new_texture.m_width;

	m_texture.load();

	m_mesh.vertices.resize(m_texture.m_width * m_texture.m_height);
	m_mesh.faces.resize((m_texture.m_width - 1) * (m_texture.m_height - 1) * 6);

	size_t tri_index = 0;
	unsigned width = m_texture.m_width;
	unsigned height= m_texture.m_height;
	auto add_tri = [&](unsigned a, unsigned b, unsigned c)
	{
		m_mesh.faces[tri_index] = a;
		m_mesh.faces[tri_index + 1u] = b;
		m_mesh.faces[tri_index + 2u] = c;
		tri_index +=3;
	};

	unsigned vtx_index = 0;
	float topLeftX = (width - 1) / -2.0f;
	float topLeftZ = (height - 1) / 2.0f;

	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			m_mesh.vertices[vtx_index] = { topLeftX + x, m_texture.get(x,y), topLeftZ - y };
			if (x < width - 1 && y < height - 1)
			{
				add_tri(vtx_index, vtx_index + width + 1, vtx_index + width);
				add_tri(vtx_index + width + 1, vtx_index, vtx_index + 1);
			}
			vtx_index++;
		}
	}
	m_mesh.load();
}
