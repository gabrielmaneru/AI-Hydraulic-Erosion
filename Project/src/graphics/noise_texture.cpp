#include "noise_texture.h"
#include <utils/generate_noise.h>
void noise_texture::generate()
{
	map2d<float> new_texture = generate_noise(resolution, noise_scale, iterations, persistance, lacunarity, 0.0f, 1.0f, falloff);
	m_texture.m_values = new_texture.m_values;
	m_texture.m_height = new_texture.m_height;
	m_texture.m_width = new_texture.m_width;

	m_texture.load();

	m_naive_mesh.vertices.resize(m_texture.m_width * m_texture.m_height);
	m_naive_mesh.faces.resize((m_texture.m_width - 1) * (m_texture.m_height - 1) * 6);

	size_t tri_index = 0;
	size_t width = m_texture.m_width;
	size_t height= m_texture.m_height;
	auto add_tri = [&](size_t a, size_t b, size_t c)
	{
		m_naive_mesh.faces[tri_index] =		 (unsigned)a;
		m_naive_mesh.faces[tri_index + 1u] = (unsigned)b;
		m_naive_mesh.faces[tri_index + 2u] = (unsigned)c;
		tri_index +=3;
	};

	size_t vtx_index = 0;
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			float x_factor = x / (width - 1.0f);
			float y_factor = y / (height - 1.0f);

			m_naive_mesh.vertices[vtx_index] = { -0.5f + x_factor, m_texture.get(x,y), 0.5f - y_factor };
			if (x < width - 1 && y < height - 1)
			{
				add_tri(vtx_index, vtx_index + width + 1, vtx_index + width);
				add_tri(vtx_index + width + 1, vtx_index, vtx_index + 1);
			}
			vtx_index++;
		}
	}
	//m_water.vertices = m_naive_mesh.vertices;
	//m_water.faces = m_naive_mesh.faces;
	//for (auto& i : m_water.vertices)
	//	i.y = 10.f;

	m_naive_mesh.load();
	//m_water.load();
}
