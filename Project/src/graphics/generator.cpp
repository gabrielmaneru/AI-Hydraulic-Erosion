#include "generator.h"
#include "shader_program.h"
#include <utils/generate_noise.h>
#include <imgui/imgui.h>

void generator::init()
{
	change_step(s_select_noise_map);
}

void generator::update()
{
}

void generator::set_uniforms(Shader_Program * shader_p, e_shader shader_type)
{
	if (shader_type == e_shader::e_color_mesh || shader_type == e_shader::e_mesh)
	{
		for (int i = 0; i < levels.size(); ++i)
		{
			if(shader_type == e_shader::e_color_mesh)
				shader_p->set_uniform(("levels[" + std::to_string(i) + "].color").c_str(), levels[i].color);
			shader_p->set_uniform(("levels[" + std::to_string(i) + "].txt_height").c_str(), levels[i].txt_height);
			shader_p->set_uniform(("levels[" + std::to_string(i) + "].real_height").c_str(), levels[i].real_height);
		}
		assert(levels.size() <= 10);

		shader_p->set_uniform("active_levels", (int)levels.size());
		shader_p->set_uniform("terrain_slope", terrain_slope);

		if (shader_type == e_shader::e_mesh)
		{
			shader_p->set_uniform("z_off", 0.01f);
		}
		else
		{
			shader_p->set_uniform("blend_factor", blend_factor);
			shader_p->set_uniform("z_off", 0.00f);
		}

	}
}

void generator::draw_gui()
{
	if (ImGui::Button("Prev") && step > s_select_noise_map)
		return change_step(step - 1);
	ImGui::SameLine(0);
	if (ImGui::Button("Next") && step < s_rasterization)
		return change_step(step + 1);
	ImGui::Text("Options:");

	switch (step)
	{
	case s_select_noise_map:
		{
			bool changed = false;
			if (ImGui::SliderFloat("Noise Scale", &m_noise.noise_scale, 0.0f, 10.0f))changed = true;
			if (ImGui::InputInt("Iterations", &m_noise.iterations))changed = true;
			if (ImGui::SliderFloat("Complexity", &m_noise.lacunarity, 0.5f, 2.0f))m_noise.persistance = 1 / m_noise.lacunarity, changed = true;
			if (ImGui::SliderFloat("FallOff", &m_noise.falloff, 0.001f, 20.0f))changed = true;
			if (ImGui::Button("Random"))
				changed = true, randomize_noise();
			if (ImGui::InputUInt("Preview Resolution", &m_noise.resolution))
				changed = true;
			if (changed)
				m_noise.generate();

			ImGui::InputUInt("Resolution", &m_noise.post_resolution);
			ImGui::InputFloat("Scale", &display_scale);
		}
		break;
	case s_apply_layers:
		{
			ImGui::SliderFloat("Blendfactor", &blend_factor, -2.0f, 2.0f);
			ImGui::SliderFloat("Slope", &terrain_slope, 0.0f, 3.0f);
			for (int i = 0; i < levels.size(); ++i)
			{
				if (ImGui::TreeNode(("Layer_" + std::to_string(i)).c_str()))
				{
					ImGui::ColorEdit3("Color", &levels[i].color[0]);
					ImGui::SliderFloat("Influence", &levels[i].txt_height, 0.0f, 1.0f);
					ImGui::SliderFloat("Height", &levels[i].real_height, 0.0f, 500.0f);
					ImGui::TreePop();
				}
			}
		}
		break;
	default:
		break;
	}
		
		
}

void generator::rasterize_mesh()
{
	m_rasterized_mesh.faces = m_layer_mesh.faces;
	m_rasterized_mesh.vertices = m_layer_mesh.vertices;
	m_rasterized_mesh.uv_coord.resize(m_layer_mesh.vertices.size());
	m_rasterized_txt.setup(m_noise.resolution, m_noise.resolution);
	m_rasterized_txt.clear({});

	for (size_t y = 0; y < m_noise.resolution; y++)
		for (size_t x = 0; x < m_noise.resolution; x++)
		{
			vec3& vertex = m_rasterized_mesh.vertices[y*m_noise.resolution + x];
			vec2& uv = m_rasterized_mesh.uv_coord[y*m_noise.resolution + x];
			float value = vertex.y;

			// Get level
			int current=0;
			for (; current < levels.size(); current++)
				if (value <= levels[current].txt_height)
					break;

			// Get Acc height
			float acc = 0;
			for (int i = 0; i < current; i++)
				acc += levels[i].real_height;


			// Get base level
			float prev_level = 0.0;
			if (current > 0)
				prev_level = levels[current - 1].txt_height;

			// Get current level ratio
			float level_ratio = 0.0;
			float level_size = levels[current].txt_height - prev_level;
			if (level_size > 0.0)
				level_ratio = (value - prev_level) / level_size;

			// Curve it
			level_ratio = 1 - pow(1 - level_ratio, terrain_slope);

			// Compute uvs
			uv.x = map(vertex.x, -0.5f, 0.5f, 0.0f, 1.0f);
			uv.y = map(vertex.z, 0.5f, -0.5f, 0.0f, 1.0f);

			// Compute real height
			vertex.x *= display_scale;
			vertex.y = acc + levels[current].real_height * level_ratio;
			vertex.z *= display_scale;

			vec3 level_color = levels[current].color;

			float prev = 0.0, post = 1.0;
			if (current > 0)
				prev = levels[current - 1].txt_height;

			if (current < levels.size() - 1)
				post = levels[current].txt_height;

			float factor = (value - prev) / (post - prev);
			if (factor < blend_factor && current > 0)
			{
				level_color += (levels[current - 1].color - level_color) * ((blend_factor - factor) / (blend_factor * 2));
			}
			if ((1 - factor) < blend_factor && (current < levels.size() - 1))
			{
				level_color += (levels[current + 1].color - level_color) * ((blend_factor - (1 - factor)) / (blend_factor * 2));
			}

			m_rasterized_txt.set(x, y, level_color);
		}
	m_rasterized_mesh.compute_terrain_normals();
	m_rasterized_mesh.load();
	m_rasterized_txt.load();
}

void generator::enter_step()
{
	switch (step)
	{
	case s_select_noise_map:
		m_noise.resolution = m_noise.prev_resolution;
		m_noise.generate();
		break;
	case s_apply_layers:
		break;
	case s_rasterization:
		rasterize_mesh();
		break;
	default:
		break;
	}
}

void generator::exit_step()
{
	switch (step)
	{
	case s_select_noise_map:
		m_noise.resolution = m_noise.post_resolution;
		m_noise.generate();
		m_layer_mesh = m_noise.m_naive_mesh;
		m_noise.m_naive_mesh = {};
		break;
	case s_apply_layers:
		break;
	case s_rasterization:
		break;
	default:
		break;
	}
}

void generator::change_step(int next_step)
{
	exit_step();
	step = next_step;
	enter_step();
}
