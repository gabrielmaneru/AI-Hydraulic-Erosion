#pragma once
#include "noise_texture.h"

constexpr int s_select_noise_map = 0;
constexpr int s_apply_layers = 1;

class Shader_Program;
struct generator
{
	noise_texture m_noise;
	raw_mesh m_mesh;
	float display_scale{ 2000.0f };
	float blend_factor{ 0.75f };
	float terrain_slope{ 0.75f };

	struct level
	{
		vec3 color;
		float txt_height;
		float real_height;
	};
	std::vector<level> levels
	{
		{ vec3{ 0.941f, 0.941f, 0.471f }, 0.4f, 75.f },
		{ vec3{ 0.118f, 0.471f, 0.235f }, 0.7f, 100.f },
		{ vec3{ 0.275f, 0.234f, 0.078f }, 0.9f, 100.f },
		{ vec3{ 0.196f, 0.196f, 0.196f }, 1.0f, 50.f }
	};

	void init();
	void update();
	enum class e_shader {e_color_mesh, e_mesh};
	void set_uniforms(Shader_Program* shader_p, e_shader shader_type);
	void draw_gui();

	int step{ -1 };
private:
	void enter_step();
	void exit_step();
	void change_step(int next_step);
};