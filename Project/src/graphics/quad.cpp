#include "quad.h"
#include "gl_error.h"
#include <vector>
#include <glm/glm.h>
#include <GL/gl3w.h>
s_quad quad{};

void s_quad::load()
{
	std::vector<vec3> positions = {
		{ -0.5f, -0.5f, 0.0f },
		{ 0.5f,  -0.5f, 0.0f },
		{ 0.5f,  0.5f,  0.0f },

		{ -0.5f, 0.5f,  0.0f },
		{ -0.5f, -0.5f, 0.0f },
		{ 0.5f,  0.5f,  0.0f },
	};

	std::vector<vec2> uvs = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1.0f, 1.0f },
	};

	// VAO
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
	cnt = 6;
}
