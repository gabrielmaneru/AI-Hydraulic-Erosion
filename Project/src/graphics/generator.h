#pragma once
#include "noise_texture.h"
#include "framebuffer.h"
#include "eroder.h"

constexpr int s_select_noise_map = 0;
constexpr int s_apply_layers = 1;
constexpr int s_rasterization = 2;

class Shader_Program;
struct generator
{
	// Noiser
	noise_texture m_noise;
	bool m_shadowy = true;

	// Layer
	raw_mesh m_layered_mesh;
	rasterized_data m_rasterized;
	float m_blend_factor{ 0.5f };
	float m_terrain_slope{ 0.75f };
	float m_water_height{ 0.05f };

	// Water
	float m_reflect_factor{ 0.55f };
	float m_wcolor_factor{ 0.45f };
	vec3 m_wcolor{ 0.12f, 0.45f, 0.80f };
	framebuffer m_reflection;
	framebuffer m_refraction;

	// Eroder
	eroder m_eroder;

	struct level
	{
		vec3 color;
		float txt_height;
		float real_height;
	};
	std::vector<level> levels
	{
		{ vec3{ 1.00f, 1.00f, 0.54f }, 0.25f, 0.05f },
		{ vec3{ 0.07f, 0.32f, 0.15f }, 0.5f, 0.05f },
		{ vec3{ 0.27f, 0.23f, 0.07f }, 0.75f, 0.05f },
		{ vec3{ 1.00f, 1.00f, 1.00f }, 1.0f, 0.05f }
	};

	void init();
	void update();
	enum class e_shader {e_layer_color, e_layer_mesh, e_water};
	void set_uniforms(Shader_Program* shader_p, e_shader shader_type);
	void draw_gui();

	int step{ -1 };
	void rasterize_mesh();
	void rasterize_texture();

private:

	void enter_step();
	void exit_step();
	void change_step(int next_step);
};