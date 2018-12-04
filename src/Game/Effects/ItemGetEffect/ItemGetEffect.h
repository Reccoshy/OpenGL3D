#pragma once

#include <glm/glm.hpp>
#include <vector>

class CSparcleEffect {
public:
	bool Init(glm::vec3 pos, glm::vec4 color, int emitNum);

	void Update(float delta);

	bool IsActive();

private:
	
	void Draw();

private:

	int m_emitNum = 0;

	std::vector<glm::vec3> m_potition;

	std::vector<glm::vec3> m_velocity;

	float m_randMaxVel = 50.0f;
	float m_randMinVel = -50.0f;

	std::vector<float> m_scales;

	float m_randMaxScale = 100.0f;
	float m_randMinScale = 50.0f;

	glm::vec4 m_color;


	float m_lifeTime = 0.5f;
	float m_time = 0.0f;

	bool m_active = false;
};