#pragma once
#include "shader_program.h"
#include "vectorial_camera.h"
#include "ortho_camera.h"
#include "noise_texture.h"
class c_renderer
{
	Shader_Program* terrain_shader;
	Shader_Program* terrain_mesh_shader;
	vectorial_camera scene_cam;
	noise_texture m_noise;
	Shader_Program* water_shader;
	Shader_Program* texture_shader;
	ortho_camera ortho_cam;

	struct
	{
		uint32_t vao{ 0 };
		uint32_t cnt{};
	} quad{};

public:
	bool init();
	void update();
	void shutdown();
	friend class c_editor;
};
extern c_renderer* renderer;