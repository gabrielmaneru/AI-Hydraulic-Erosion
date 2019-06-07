#include "renderer.h"
#include "gl_error.h"
#include "quad.h"
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
		texture_shader = new Shader_Program("resources/shaders/texture.vert", "resources/shaders/texture.frag");
		noise_mesh_shader = new Shader_Program("resources/shaders/noise_mesh.vert", "resources/shaders/color.frag");
		layer_shader = new Shader_Program("resources/shaders/layer.vert", "resources/shaders/layer.frag");
		layer_mesh_shader = new Shader_Program("resources/shaders/layer.vert", "resources/shaders/layer_mesh.frag");
		//water_shader = new Shader_Program("resources/shaders/water.vert", "resources/shaders/color.frag");
	}
	catch (const std::string & log) { std::cout << log; return false; }

	//Load Resources
	scene_cam.m_eye = { .0f, 500.0f, 800.0f };
	scene_cam.m_yaw = -90.0f;
	scene_cam.m_pitch = -40.0f;
	scene_cam.update_cam_vectors();

	ortho_cam.view_rect = {-0.5f, 0.5f, -0.5f, 0.5f};
	ortho_cam.update();

	m_generator.init();

	quad.load();
	return true;
}

void c_renderer::update()
{
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GL_CALL(glViewport(0, 0, window_manager->get_width(), window_manager->get_height()));

	// Camera Update
	scene_cam.update(window::mouse_offset[0], window::mouse_offset[1]);
	mat4 mvp;

	switch (m_generator.step)
	{
	case s_select_noise_map:
		{// Draw Mesh
			mvp = scene_cam.m_proj * scene_cam.m_view * glm::scale(mat4(1.0f), { m_generator.display_scale, m_generator.display_scale/4, m_generator.display_scale });
			noise_mesh_shader->use();
			noise_mesh_shader->set_uniform("MVP", mvp);
			noise_mesh_shader->set_uniform("base_color", vec4{ 1.0f, 0.0f, 1.0f, 1.0f });
			// Draw Scene
			GL_CALL(glEnable(GL_DEPTH_TEST));
			GL_CALL(glBindVertexArray(m_generator.m_noise.m_naive_mesh.m_vao));
			GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
			GL_CALL(glDrawElements(GL_TRIANGLES, m_generator.m_noise.m_naive_mesh.faces.size(), GL_UNSIGNED_INT, 0));
			GL_CALL(glDisable(GL_DEPTH_TEST));
		}
		{// Draw Noise Image
			GL_CALL(glViewport(window_manager->get_width()*(4.0f / 5.0f), 0, window_manager->get_width() / 5.0f, window_manager->get_height() / 5.0f));
			
			texture_shader->use();
			texture_shader->set_uniform("MVP", ortho_cam.m_proj * ortho_cam.m_view);
			GL_CALL(glActiveTexture(GL_TEXTURE0));
			GL_CALL(glBindTexture(GL_TEXTURE_2D, m_generator.m_noise.m_texture.m_id));
			GL_CALL(glUniform1i(m_generator.m_noise.m_texture.m_id, 0));

			// Draw Quad
			GL_CALL(glBindVertexArray(quad.vao));
			GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
			GL_CALL(glDrawArrays(GL_TRIANGLES, 0, quad.cnt));
		}
		break;

	case s_apply_layers:
		{// Draw Terrain
			mvp = scene_cam.m_proj * scene_cam.m_view * glm::scale(mat4(1.0f), { m_generator.display_scale, 1.0f, m_generator.display_scale });
			layer_shader->use();
			layer_shader->set_uniform("MVP", mvp);
			m_generator.set_uniforms(layer_shader, generator::e_shader::e_color_mesh);

			// Draw Scene
			GL_CALL(glEnable(GL_DEPTH_TEST));
			GL_CALL(glBindVertexArray(m_generator.m_mesh.m_vao));
			GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
			GL_CALL(glDrawElements(GL_TRIANGLES, m_generator.m_mesh.faces.size(), GL_UNSIGNED_INT, 0));
		}
		{// Draw Terrain Mesh
			layer_mesh_shader->use();
			layer_mesh_shader->set_uniform("MVP", mvp);
			layer_mesh_shader->set_uniform("base_color", vec4{ 0.0f, 0.0f, 0.0f, 1.0f });
			m_generator.set_uniforms(layer_mesh_shader, generator::e_shader::e_mesh);
			GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
			GL_CALL(glDrawElements(GL_TRIANGLES, m_generator.m_mesh.faces.size(), GL_UNSIGNED_INT, 0));
			GL_CALL(glDisable(GL_DEPTH_TEST));
		}
		break;
	default:
		break;
	}
}

void c_renderer::shutdown()
{
	//Clean Resouces

}
