#pragma once
#include "../../../../Entity.h"

class RaceScene;

class Smoke {
public:
	bool Init(glm::vec3 pos, float rot, RaceScene* p);

	void Update(float delta);

	glm::vec3 Position()
	{
		return m_pEntity->Position();
	}

	void Destroy() {
		m_pEntity->Destroy();
	}

	bool IsActive() {
		return m_isActive;
	}

private:

	Entity::Entity* m_pEntity = nullptr;

	bool m_isActive = true;

	float m_scale = 0.0f;

	float m_time = 0.0f;
	float m_smokeTime = 10.0f;
	float m_spawnTime = 1.0f;
};