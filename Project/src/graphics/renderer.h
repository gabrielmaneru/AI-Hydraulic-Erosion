#pragma once
#include "shader_program.h"
#include "vectorial_camera.h"
#include "ortho_camera.h"
#include "generator.h"
class c_renderer
{
	// Shaders
	Shader_Program* texture_shader;
	Shader_Program* basic_shader;
	Shader_Program* layer_shader;
	Shader_Program* layer_mesh_shader;
	Shader_Program* gradient_shader;

	// Cameras
	vectorial_camera scene_cam;
	ortho_camera ortho_cam;

	generator m_generator;

public:
	bool init();
	void update();
	void shutdown();
	friend class c_editor;
};
extern c_renderer* renderer;