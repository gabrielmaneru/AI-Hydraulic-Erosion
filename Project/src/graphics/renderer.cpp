#include "renderer.h"
#include "gl_error.h"
#include <platform/window_manager.h>
#include <platform/window.h>
#include <scene/scene.h>
#include <utils/generate_noise.h>
#include <GL/gl3w.h>
#include <iostream>
#include <algorithm>

c_renderer* renderer = new c_renderer;

bool c_renderer::init()
{
	if (gl3wInit())
		return false;
	
	if (!gl3wIsSupported(4, 0))
		return false;

	// GL Options
	GL_CALL(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GL_CALL(glEnable(GL_CULL_FACE));

	//Load Programs
	try {
		terrain_shader = new Shader_Program("resources/shaders/terrain.vert", "resources/shaders/terrain.frag");
		color_shader = new Shader_Program("resources/shaders/color.vert", "resources/shaders/color.frag");
		texture_shader = new Shader_Program("resources/shaders/texture.vert", "resources/shaders/texture.frag");
	}
	catch (const std::string & log) { std::cout << log; return false; }

	//Load Resources
	scene_cam.m_eye = { .0f, 500.0f, 800.0f };
	scene_cam.m_yaw = -90.0f;
	scene_cam.m_pitch = -40.0f;
	scene_cam.update_cam_vectors();

	ortho_cam.view_rect = {-0.5f, 0.5f, -0.5f, 0.5f};

	m_noise.update();
	//Load Meshes
	{ // Load Quad
		std::vector<vec3> positions = {
					{-0.5f, -0.5f, 0.0f},
					{0.5f,  -0.5f, 0.0f},
					{0.5f,  0.5f,  0.0f},

					{-0.5f, 0.5f,  0.0f},
					{-0.5f, -0.5f, 0.0f},
					{0.5f,  0.5f,  0.0f},
		};

		std::vector<vec2> uvs = {
				{0.0f, 0.0f},
				{1.0f, 0.0f},
				{1.0f, 1.0f},
				{0.0f, 1.0f},
				{0.0f, 0.0f},
				{1.0f, 1.0f},
		};

		// VAO
		uint32_t vao{};
		GL_CALL(glGenVertexArrays(1, &vao));
		GL_CALL(glBindVertexArray(vao));

		// Vertices
		uint32_t vbo_vertices{};
		GL_CALL(glGenBuffers(1, &vbo_vertices));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * positions.size(), positions.data(), GL_STATIC_DRAW));

		// UV
		uint32_t vbo_uv{};
		GL_CALL(glGenBuffers(1, &vbo_uv));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo_uv));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * uvs.size(), uvs.data(), GL_STATIC_DRAW));


		// Positions
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices));
		GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr));
		GL_CALL(glEnableVertexAttribArray(0));

		// UV
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo_uv));
		GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr));
		GL_CALL(glEnableVertexAttribArray(1));
		GL_CALL(glBindVertexArray(0));

		quad.vao = vao;
		quad.cnt = 6;
	}
	return true;
}

void c_renderer::update()
{
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GL_CALL(glViewport(0, 0, window_manager->get_width(), window_manager->get_height()));

	// Scene Pass
	{
		// Camera Update
		scene_cam.update(window::mouse_offset[0], window::mouse_offset[1]);
		mat4 mvp = scene_cam.m_proj * scene_cam.m_view * glm::scale(mat4(1.0f), { m_noise.display_scale, m_noise.display_scale / 100.0f * m_noise.display_height, m_noise.display_scale });

		// Set shader
		terrain_shader->use();
		terrain_shader->set_uniform("MVP", mvp);
		for (int i = 0; i < m_noise.levels.size(); ++i)
		{
			terrain_shader->set_uniform(("levels[" + std::to_string(i) + "].color").c_str(), m_noise.levels[i].color);
			terrain_shader->set_uniform(("levels[" + std::to_string(i) + "].txt_height").c_str(), m_noise.levels[i].txt_height);
			terrain_shader->set_uniform(("levels[" + std::to_string(i) + "].real_height").c_str(), m_noise.levels[i].real_height);
		}
		assert(m_noise.levels.size() <= 10);
		terrain_shader->set_uniform("active_levels", (int)m_noise.levels.size());
		terrain_shader->set_uniform("blend_factor", m_noise.blend_factor);
		terrain_shader->set_uniform("terrain_slope", m_noise.terrain_slope);

		// Draw Scene
		GL_CALL(glEnable(GL_DEPTH_TEST));
		GL_CALL(glBindVertexArray(m_noise.m_mesh.m_vao));

		GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		GL_CALL(glDrawElements(GL_TRIANGLES, m_noise.m_mesh.faces.size(), GL_UNSIGNED_INT, 0));

		color_shader->use();
		color_shader->set_uniform("MVP", mvp);
		color_shader->set_uniform("base_color", vec4{0.0f, 0.0f, 0.0f, 1.0f});
		for (int i = 0; i < m_noise.levels.size(); ++i)
		{
			color_shader->set_uniform(("levels[" + std::to_string(i) + "].txt_height").c_str(), m_noise.levels[i].txt_height);
			color_shader->set_uniform(("levels[" + std::to_string(i) + "].real_height").c_str(), m_noise.levels[i].real_height);
		}
		assert(m_noise.levels.size() <= 10);
		color_shader->set_uniform("active_levels", (int)m_noise.levels.size());
		color_shader->set_uniform("terrain_slope", m_noise.terrain_slope);
		GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
		GL_CALL(glDrawElements(GL_TRIANGLES, m_noise.m_mesh.faces.size(), GL_UNSIGNED_INT, 0));
		GL_CALL(glDisable(GL_DEPTH_TEST));
	}

	GL_CALL(glViewport(window_manager->get_width()*(4.0f/5.0f), 0, window_manager->get_width() / 5.0f, window_manager->get_height() / 5.0f));
	// HUD Pass
	{
		// Camera Update
		ortho_cam.update();
		mat4 mvp = ortho_cam.m_proj * ortho_cam.m_view;

		// Set shader
		texture_shader->use();
		texture_shader->set_uniform("MVP", mvp);
		GL_CALL(glActiveTexture(GL_TEXTURE0));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, m_noise.m_texture.m_id));
		GL_CALL(glUniform1i(m_noise.m_texture.m_id, 0));

		// Draw Quad

		GL_CALL(glBindVertexArray(quad.vao));
		GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, quad.cnt));
	}

}

void c_renderer::shutdown()
{
	//Clean Resouces

}
