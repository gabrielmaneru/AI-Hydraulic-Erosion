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
	enum dbg_mesh{ cube=0, octohedron, quad, segment, sphere };
	struct dbg_shape
	{
		dbg_mesh mesh;
		vec3 pos;
		vec3 scale;
		vec4 f_color;
		vec4 l_color;
	};
	std::vector<dbg_shape> dbg_shapes;
	size_t debug_draw_cube(vec3 pos, vec3 scale, vec4 f_color, vec4 l_color = vec4{ 0.0f });
	size_t debug_draw_octohedron(vec3 pos, vec3 scale, vec4 f_color, vec4 l_color = vec4{ 0.0f });
	size_t debug_draw_quad(vec3 pos, vec3 scale, vec4 f_color, vec4 l_color = vec4{ 0.0f });
	size_t debug_draw_line(vec3 p0, vec3 p1, vec4 l_color = vec4{ 0.0f });
	size_t debug_draw_sphere(vec3 pos, float rad, vec4 f_color, vec4 l_color = vec4{ 0.0f });
	void debug_draw_remove(size_t idx);
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