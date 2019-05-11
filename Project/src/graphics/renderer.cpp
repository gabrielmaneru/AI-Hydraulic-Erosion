#include "renderer.h"
#include "gl_error.h"
#include <platform/window_manager.h>
#include <platform/window.h>
#include <scene/scene.h>
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

	//Load Programs
	try { shader = new Shader_Program("resources/shaders/color.vert", "resources/shaders/color.frag"); }
	catch (const std::string & log) { std::cout << log; return false; }

	//Load Resources
	scene_cam.m_eye = { 0.0f, 0.0f, 3.0f };
	scene_cam.m_yaw = -90.0f;
	scene_cam.m_pitch = 0.0f;
	scene_cam.update_cam_vectors();

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

		// VAO
		uint32_t vao{};
		GL_CALL(glGenVertexArrays(1, &vao));
		GL_CALL(glBindVertexArray(vao));

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

	// Camera Update
	scene_cam.update(window::mouse_offset[0], window::mouse_offset[1]);

	// Set shader
	shader->use();

	mat4 mvp = scene_cam.m_proj * scene_cam.m_view;
	shader->set_uniform("MVP", mvp);

	// Bind mesh
	GL_CALL(glBindVertexArray(quad.vao));

	// Draw fill
	GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, quad.cnt));
}

void c_renderer::shutdown()
{
	//Clean Resouces

}
