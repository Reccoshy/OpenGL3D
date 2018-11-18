#pragma once
#include <glm/glm.hpp>

class RespawnEffect
{
public:
	bool Init(glm::vec3 pos, float time);

	void Update(float delta);

	void Draw();

	bool IsActive()
	{
		return m_active;
	}

	glm::vec3 Position()
	{
		return m_position;
	}

private:

	bool m_active = false;
	
	glm::vec3 m_velocity;

	glm::vec3 m_position;
	glm::vec2 m_scale;
	glm::vec4 m_color;

	float m_yRot = 0.0f;

	int m_animeNum = 1;
	int m_lastAnimeNum = 9;
	float m_animaChangeTime = 0.05f;
	float m_currentAnimTime = 0.0f;

	float m_lifeTime = 2.0f;
	float m_time = 0.0f;
};