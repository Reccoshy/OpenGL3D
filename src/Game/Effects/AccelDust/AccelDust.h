#pragma once
#include <glm/glm.hpp>

class AccelDust 
{
public:

	bool Init(glm::vec3 pos);

	void Update(float dt);

	void Draw();

	bool ActiveCheck() { return m_active; }

private:
	float m_scale = 20.0f;
	float m_endScale = 25.0f;

	float m_scaleMultiplier = 10.0f;

	glm::vec3 m_position;

	glm::vec4 m_color = glm::vec4(0.5f, 0.5f, 0.0f, 0.5f);

	bool m_active = false;
};
