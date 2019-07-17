#include "generator.h"
#include "shader_program.h"
#include <utils/generate_noise.h>
#include <imgui/imgui.h>
#include "renderer.h"

void generator::init()
{
	change_step(s_select_noise_map);
}

void generator::update()
{
}

void generator::set_uniforms(Shader_Program * shader_p, e_shader shader_type)
{
	switch (shader_type)
	{
	case generator::e_shader::e_layer_color:
	case generator::e_shader::e_layer_mesh:
		for (int i = 0; i < levels.size(); ++i)
		{
			if(shader_type == e_shader::e_layer_color)
				shader_p->set_uniform(("levels[" + std::to_string(i) + "].color").c_str(), levels[i].color);
			shader_p->set_uniform(("levels[" + std::to_string(i) + "].txt_height").c_str(), levels[i].txt_height);
			shader_p->set_uniform(("levels[" + std::to_string(i) + "].real_height").c_str(), levels[i].real_height);
		}
		assert(levels.size() <= 10);

		shader_p->set_uniform("active_levels", (int)levels.size());
		shader_p->set_uniform("terrain_slope", m_terrain_slope);

		if (shader_type == e_shader::e_layer_mesh)
		{
			shader_p->set_uniform("useColor", true);
			shader_p->set_uniform("base_color", vec4(0.1f, 0.1f, 0.1f, 1.0f));
			shader_p->set_uniform("z_off", FLT_EPSILON);
		}
		else
		{
			shader_p->set_uniform("useColor", false);
			shader_p->set_uniform("blend_factor", m_blend_factor);
			shader_p->set_uniform("z_off", 0.00f);
		}
		break;
	case generator::e_shader::e_water:
		mat4 mtx{ 1.0f };
		mtx = glm::translate(mtx, { 0.0f, m_water_height, 0.0f });
		mtx = glm::rotate(mtx, -glm::pi<float>() / 2, { 1.0f, 0.0f, 0.0f });
		mtx = glm::scale(mtx, vec3(5, 5, 0));
		shader_p->set_uniform("Model", mtx);
		break;
	default:
		break;
	}
}

void generator::draw_gui()
{
	switch (step)
	{
	case s_select_noise_map:
		ImGui::Text("Step: Selecting Noise Map");
		ImGui::NewLine();
		ImGui::SameLine(108);
		//if (ImGui::Button("Prev", {60,20}))
		//	return change_step(step - 1);
		//ImGui::SameLine();
		if (ImGui::Button("Next: Layers", {80,20}))
			return change_step(s_apply_layers);
		break;
	case s_apply_layers:
		ImGui::Text("Step: Selecting Layers");
		ImGui::NewLine();
		ImGui::SameLine(20);
		if (ImGui::Button("Prev: Noise", { 80,20 }))
			return change_step(s_select_noise_map);
		ImGui::SameLine();
		if (ImGui::Button("Next: Erosion", { 80,20 }))
			return change_step(s_rasterization);
		break;
	case s_rasterization:
		ImGui::Text("Step: Erosion");
		ImGui::NewLine();
		ImGui::SameLine(20);
		if (ImGui::Button("Prev: Layers", { 80,20 }))
			return change_step(s_apply_layers);
		//ImGui::SameLine();
		//if (ImGui::Button("Next", { 60,20 }))
		//	return change_step(step - 1);
		break;
	default:
		break;
	}
	ImGui::NewLine();
	ImGui::NewLine();
	
	switch (step)
	{
	case s_select_noise_map:
		{
			bool changed = false;
			if (ImGui::TreeNode("Options:"))
			{
				if (ImGui::SliderFloat("Noise Scale", &m_noise.noise_scale, 0.0f, 10.0f))changed = true;
				if (ImGui::InputInt("Iterations", &m_noise.iterations))changed = true;
				if (ImGui::SliderFloat("Lacunarity", &m_noise.lacunarity, 0.0f, 2.0f))changed = true;
				if (ImGui::SliderFloat("Persistance", &m_noise.persistance, 0.0f, 2.0f))changed = true;
				if (ImGui::SliderFloat("FallOff Intensity", &m_noise.falloff.x, 0.5f, 5.0f))changed = true;
				if (ImGui::SliderFloat("FallOff Form", &m_noise.falloff.y, -1.0f, 1.0f))changed = true;
				if (ImGui::Button("Random"))
					changed = true, randomize_noise();
				if (ImGui::InputUInt("Preview Resolution", &m_noise.resolution))
					changed = true;
				ImGui::InputUInt("Resolution", &m_noise.post_resolution);
				ImGui::Checkbox("Shadows", &m_shadowy);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Maps:"))
			{
				if (ImGui::Button("BASIC 1", { 200, 100 }))
				{
					m_noise.noise_scale=5.0f;
					m_noise.iterations=8;
					m_noise.lacunarity=2.0f;
					m_noise.persistance =0.5f;
					m_noise.falloff = { 0.5f,0.25f };
					changed = true;
				}ImGui::SameLine();
				if (ImGui::Button("BASIC 2", { 200, 100 }))
				{
					m_noise.noise_scale = 5.0f;
					m_noise.iterations = 8;
					m_noise.lacunarity = 2.0f;
					m_noise.persistance = 0.0f;
					m_noise.falloff = { 0.5f,0.25f };
					changed = true;
				}

				if (ImGui::Button("ISLE 1", { 200, 100 }))
				{
					m_noise.noise_scale = 10.f;
					m_noise.iterations = 8;
					m_noise.lacunarity = 2.0f;
					m_noise.persistance = 0.5f;
					m_noise.falloff = { 0.5f, 1.0f };
					changed = true;
				}ImGui::SameLine();
				if (ImGui::Button("ISLE 2", { 200, 100 }))
				{
					m_noise.noise_scale = 10.f;
					m_noise.iterations = 8;
					m_noise.lacunarity = 2.0f;
					m_noise.persistance = 0.0f;
					m_noise.falloff = { 0.5f, 1.0f };
					changed = true;
				}
				if (ImGui::Button("LAKES 1", { 200, 100 }))
				{
					m_noise.noise_scale = 10.f;
					m_noise.iterations = 8;
					m_noise.lacunarity = 2.0f;
					m_noise.persistance = 0.5f;
					m_noise.falloff = { 0.5f, -1.0f };
					changed = true;
				}ImGui::SameLine();
				if (ImGui::Button("LAKES 2", { 200, 100 }))
				{
					m_noise.noise_scale = 10.f;
					m_noise.iterations = 8;
					m_noise.lacunarity = 2.0f;
					m_noise.persistance = 0.0f;
					m_noise.falloff = { 0.5f, -1.0f };
					changed = true;
				}
				ImGui::TreePop();
			}

			if (changed)
				m_noise.generate();

		}
		break;
	case s_apply_layers:
		{
			ImGui::SliderFloat("Blendfactor", &m_blend_factor, -2.0f, 2.0f);
			ImGui::SliderFloat("Slope", &m_terrain_slope, 0.0f, 3.0f);
			ImGui::SliderFloat("Water Level", &m_water_height, 0.0f, this->levels[0].real_height);

			if (ImGui::Button("Reset", { 100, 40 }))
			{
				m_blend_factor = 0.5f;
				m_terrain_slope = 0.7f;
				m_water_height = 0.03f;
				levels=
				{
					{ vec3{ 1.00f, 1.00f, 0.54f }, 0.25f, 0.05f },
					{ vec3{ 0.07f, 0.32f, 0.15f }, 0.5f, 0.05f },
					{ vec3{ 0.27f, 0.23f, 0.07f }, 0.75f, 0.05f },
					{ vec3{ 1.00f, 1.00f, 1.00f }, 1.0f, 0.05f }
				};
			}

			ImGui::Text("Sand");
			ImGui::PushID("Sand");
			ImGui::ColorEdit3("Color", &levels[0].color[0]);
			if (ImGui::SliderFloat("Influence", &levels[0].txt_height, 0.0f, 1.0f))
				levels[0].txt_height = min(levels[0].txt_height, levels[1].txt_height);
			ImGui::SliderFloat("Height", &levels[0].real_height, 0.00f, 0.2f);
			ImGui::PopID();

			ImGui::Text("Grass");
			ImGui::PushID("Grass");
			ImGui::ColorEdit3("Color", &levels[1].color[0]);
			if (ImGui::SliderFloat("Influence", &levels[1].txt_height, 0.0f, 1.0f))
				levels[1].txt_height = glm::clamp(levels[1].txt_height, levels[0].txt_height, levels[2].txt_height);
			ImGui::SliderFloat("Height", &levels[1].real_height, 0.00f, 0.2f);
			ImGui::PopID();

			ImGui::Text("Mountains");
			ImGui::PushID("Mountains");
			ImGui::ColorEdit3("Color", &levels[2].color[0]);
			if (ImGui::SliderFloat("Influence", &levels[2].txt_height, 0.0f, 1.0f))
				levels[2].txt_height = glm::clamp(levels[2].txt_height, levels[1].txt_height, levels[3].txt_height);
			ImGui::SliderFloat("Height", &levels[2].real_height, 0.00f, 0.2f);
			ImGui::PopID();

			ImGui::Text("Snow");
			ImGui::PushID("Snow");
			ImGui::ColorEdit3("Color", &levels[3].color[0]);
			ImGui::SliderFloat("Height", &levels[3].real_height, 0.00f, 0.2f);
			ImGui::PopID();
		}
		break;
	case s_rasterization:
		{
			static char* items[] = { "Step-by-step", "Iterative", "One-step" };
			static int mode{ 1 };
			if (ImGui::BeginCombo("Modes", items[mode], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					bool is_selected = (items[mode] == items[n]);
					if (ImGui::Selectable(items[n], is_selected))
						mode = n, m_eroder.reset();
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			switch (mode)
			{
			case 0: //Step-by-step
				if (!m_eroder.m_eroding)
				{
					if(ImGui::Button("Start",ImVec2(40,20)))
						m_eroder.m_eroding = true,m_eroder.remaining = m_eroder.sbs_count;
					ImGui::SliderInt("Particles", &m_eroder.sbs_count, 1, 1024);
				}
				else
				{
					ImGui::NewLine();
					ImGui::SameLine(0.0, 40.0f);
					if (ImGui::Button("Step"))
						if(!m_eroder.erode(m_rasterized, 1))
							m_eroder.reset();
					ImGui::SameLine();
					if (ImGui::Button("Stop"))
						m_eroder.reset();
				}
				break;
			case 1: //Iterative
				if (m_eroder.m_eroding)
				{
					if (ImGui::Checkbox("Iterate", &m_eroder.m_eroding))
						m_eroder.reset();
					else
					{
						m_eroder.remaining = m_eroder.it_count;
						m_eroder.erode(m_rasterized, m_eroder.it_per_frame);
					}
				}
				else
				{
					ImGui::Checkbox("Iterate", &m_eroder.m_eroding);
					ImGui::SliderInt("Particles", &m_eroder.it_count, 1, 100000);
					ImGui::SliderInt("Speed", &m_eroder.it_per_frame, 1, 1000);
				}
				break;
			case 2: //One-step
				if (ImGui::Button("Start", ImVec2(40, 20)))
				{
					m_eroder.m_eroding = true;
					m_eroder.remaining = m_eroder.os_count;
					while (m_eroder.erode(m_rasterized, m_eroder.os_count));
					m_eroder.reset();
				}
				ImGui::InputInt("Particles", &m_eroder.os_count);
				break;
			}

			if (ImGui::Button("Reset Mesh"))
			{
				rasterize_mesh();
				rasterize_texture();
			}
			ImGui::SameLine();
			if (ImGui::Button("Blur Mesh"))
				m_eroder.blur(m_rasterized);
			ImGui::SameLine();
			ImGui::SliderFloat("BlurForce", &m_eroder.blur_force, 0.0f, 1.0f);
			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::InputFloat("Inertia", &m_eroder.inertia);
			ImGui::InputFloat("Capacity Factor", &m_eroder.capacity_factor);
			ImGui::InputFloat("Deposit Factor", &m_eroder.deposit_factor);
			ImGui::InputFloat("Erode Factor", &m_eroder.erode_factor);
			ImGui::InputFloat("Evaporate Factor", &m_eroder.evaporation);
			ImGui::InputInt("Erosion Radius", &m_eroder.erosion_radius);
			ImGui::InputFloat("Gravity", &m_eroder.gravity);
			ImGui::InputInt("Lifetime", &m_eroder.max_lifetime);
			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::NewLine();
			static char* items2[] = { "Normal", "Preserve Colors", "Drag Color", "Tracer" };
			if (ImGui::BeginCombo("Display Modes", items2[m_eroder.m_display_mode], 0))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items2); n++)
				{
					bool is_selected = (items2[m_eroder.m_display_mode] == items2[n]);
					if (ImGui::Selectable(items2[n], is_selected))
					{
						m_eroder.m_display_mode = static_cast<eroder::e_display_mode>(n);
						m_eroder.reset_display();
						m_eroder.update_texture(m_rasterized);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (m_eroder.m_display_mode == eroder::tracer)
			{
				if (ImGui::Button("Reset Trace"))
				{
					m_eroder.reset_display();
					m_eroder.update_texture(m_rasterized);
				}
				if(ImGui::SliderFloat("Hardness", &m_eroder.trace_hardness, 0.05f, 1.0f))
					m_eroder.update_texture(m_rasterized);

			}
		}
		break;
	default:
		break;
	}
}

void generator::rasterize_mesh()
{
	m_rasterized.mesh.faces = m_layered_mesh.faces;
	m_rasterized.mesh.vertices = m_layered_mesh.vertices;
	m_rasterized.mesh.uv_coord.resize(m_layered_mesh.vertices.size());

	for (size_t y = 0; y < m_noise.resolution; y++)
		for (size_t x = 0; x < m_noise.resolution; x++)
		{
			vec3& vertex = m_rasterized.mesh.vertices[y*m_noise.resolution + x];
			vec2& uv = m_rasterized.mesh.uv_coord[y*m_noise.resolution + x];
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
			level_ratio = 1 - pow(1 - level_ratio, m_terrain_slope);

			// Compute uvs
			uv.x = map(vertex.x, -0.5f, 0.5f, 0.0f, 1.0f);
			uv.y = map(vertex.z, 0.5f, -0.5f, 0.0f, 1.0f);

			// Compute real height
			vertex.y = acc + levels[current].real_height * level_ratio;
		}
	m_rasterized.mesh.compute_terrain_normals();
	m_rasterized.mesh.load();
	m_rasterized.scale = static_cast<int>(m_noise.resolution);
}

void generator::rasterize_texture()
{
	m_rasterized.texture.setup(m_noise.resolution, m_noise.resolution);
	m_rasterized.texture.clear({});

	for (size_t y = 0; y < m_noise.resolution; y++)
		for (size_t x = 0; x < m_noise.resolution; x++)
		{
			float value = m_rasterized.mesh.vertices[y*m_noise.resolution + x].y;

			// Get level
			int current = 0;
			float prev = 0.0f;
			for (; current < levels.size(); current++)
				if (value <= prev + levels[current].real_height)
					break;
				else
					prev += levels[current].real_height;

			vec3 level_color = levels[current].color;

			float post = prev + levels[current].real_height;

			float factor = (value - prev) / (post - prev);
			if (factor < m_blend_factor && current > 0)
			{
				level_color += (levels[current - 1].color - level_color) * ((m_blend_factor - factor) / (m_blend_factor * 2));
			}
			if ((1 - factor) < m_blend_factor && (current < levels.size() - 1))
			{
				level_color += (levels[current + 1].color - level_color) * ((m_blend_factor - (1 - factor)) / (m_blend_factor * 2));
			}

			m_rasterized.texture.set(x, y, level_color);
		}
	m_rasterized.texture.load();
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
		rasterize_texture();
		m_reflection.setup(1920, 1080);
		m_refraction.setup(1920, 1080);
		m_eroder.initialize(m_rasterized);
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
		m_layered_mesh = m_noise.m_naive_mesh;
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
