#pragma once
#include "../../../../Entity.h"
#include "../../Shadow/Shadow.h"

class RaceScene;


class CNuke {
public:
	bool init(glm::vec3 pos, float rot, RaceScene* p);

	void Update(float delta);

	glm::vec3 Positioin()
	{
		return m_pEntity->Position();
	}

	void Destroy() {
		m_pEntity->Destroy();
		m_shadow.Destroy();
	}

	bool IsActive() {
		return isActive;
	}

private:
	Shadow m_shadow;

	Entity::Entity* m_pEntity = nullptr;

	float m_Yrot = 0.0f;

	float const StanTime = 2.0f;

	float const m_velocity = 200.0f;

	float const m_height = 0.0f;

	const float explodingTime = 0.1f;

	float m_time = 0.0f;
	float m_startFallTime = 0.5f;

	RaceScene* pRaceScene;

	bool isActive = true;
};