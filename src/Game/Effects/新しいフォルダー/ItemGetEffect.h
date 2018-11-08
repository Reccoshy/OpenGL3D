#pragma once
#include <glm/glm.hpp>

class ItemGetEffect
{
public:
	bool Init(glm::vec3 pos);


	bool IsActive() { return m_active; }

private:

	bool m_active = false;

	float m_time = 0.0f;
	float m_lifeTime = 0.5f;

	glm::vec2 m_scale = glm::vec2(10.0f);
};
