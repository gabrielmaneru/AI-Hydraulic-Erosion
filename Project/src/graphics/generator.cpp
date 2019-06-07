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
	if (ImGui::Button("Next") && step < s_apply_layers)
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
		m_mesh = m_noise.m_naive_mesh;
		m_noise.m_naive_mesh = {};
		break;
	case s_apply_layers:

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
