#pragma once
#include "shader_program.h"
#include "vectorial_camera.h"
#include "ortho_camera.h"
#include "generator.h"
#include "mesh.h"
class c_renderer
{
	// Shaders
	Shader_Program* texture_shader;
	Shader_Program* basic_shader;
	Shader_Program* layer_shader;
	Shader_Program* gradient_shader;
	Shader_Program* water_shader;

	// Cameras
	vectorial_camera scene_cam;
	vectorial_camera invert_cam;
	ortho_camera ortho_cam;

	// Scene
	std::vector<Mesh*> m_meshes;
	enum e_meshes{ cube=0, octohedron, quad, segment, sphere };
	
	generator m_generator;

public:
	bool init();
	void update();
	void shutdown();
	friend class c_editor;
	friend struct generator;
	friend class eroder;
};
extern c_renderer* renderer;