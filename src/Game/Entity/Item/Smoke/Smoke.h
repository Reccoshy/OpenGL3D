#pragma once
#include "../../../../Entity.h"

class RaceScene;

class Smoke {
public:
	bool Init(glm::vec3 pos, float yRot, RaceScene* p);

	void Update(float delta);

	glm::vec3 Position()
	{
		return m_pEntity->Position();
	}

	void Destroy() {
		m_pEntity->Destroy();
	}

	bool IsActive() {
		return isActive;
	}

private:

	Entity::Entity* m_pEntity = nullptr;

	RaceScene* m_pRaceScene = nullptr;

	float m_Yrot = 0.0f;

	bool isActive = true;

	float scale = 0.0f;

	float time = 0.0f;
	float SmokeTime = 10.0f;
	float SpawnTime = 1.0f;
};