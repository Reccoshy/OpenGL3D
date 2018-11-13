#pragma once

#include <glm/glm.hpp>

class CitemGetEffect {
public:
	bool Init(glm::vec3 pos, glm::vec4 color);

	void Update(float delta);

	void Draw();

	bool IsActive();

private:

	glm::vec3 m_potition[5];

	glm::vec3 m_velocity[5];

	glm::vec4 m_color;

	float m_randMaxVel = 50.0f;
	float m_randMinVel = -50.0f;

	float m_scale[5];

	float m_randMaxScale = 100.0f;
	float m_randMinScale = 50.0f;

	float m_lifeTime = 0.5f;
	float m_time = 0.0f;

	bool m_active = false;
};