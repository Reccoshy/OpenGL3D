#pragma once
#include <glm/glm.hpp>

class SpeedUpEffect 
{
public:
	bool Init(glm::vec3 pos, glm::vec4, glm::vec3 vel);

	void Update(float delta);

	void Draw();

	bool IsActive();

private:

	bool m_active = false;

	glm::vec3 m_velocity;

	glm::vec3 m_position;
	glm::vec2 m_scale;
	glm::vec4 m_color;

	float m_yRot = 0.0f;

	float m_lifeTime = 2.0f;
	float m_time = 0.0f;
};