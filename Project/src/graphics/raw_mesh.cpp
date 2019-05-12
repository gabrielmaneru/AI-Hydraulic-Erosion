#include "raw_mesh.h"
#include "gl_error.h"
#include <GL/gl3w.h>
void raw_mesh::load()
{
	if (m_vao == 0)
	{
		GL_CALL(glGenVertexArrays(1, &m_vao));
		GL_CALL(glBindVertexArray(m_vao));

		GL_CALL(glGenBuffers(1, &m_vtx));
		GL_CALL(glGenBuffers(1, &m_idx));
	}

	GL_CALL(glBindVertexArray(m_vao));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_vtx));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_DYNAMIC_DRAW));
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));

	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_idx));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned), &faces[0], GL_DYNAMIC_DRAW));

	GL_CALL(glBindVertexArray(0));
}