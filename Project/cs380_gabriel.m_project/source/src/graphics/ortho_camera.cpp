#include "ortho_camera.h"

void ortho_camera::update()
{
	m_view = glm::lookAt(eye, target, up);
	m_proj = glm::ortho<float>(view_rect.x, view_rect.y, view_rect.z, view_rect.w);
}