#pragma once
#include <glm/glm.hpp>
#include "../../../../Entity.h"
#include "../Item.h"
#include "../../Shadow/Shadow.h"

class RaceScene;

//�U�A�C�e��.
class FakeItem {
public:
	bool Init(glm::vec3 pos, RaceScene* pSce);

	void Update(float delta);

	void ColorUpdate();

	bool CollisionCheck(glm::vec3 pos, float radius);

	glm::vec3 Position()
	{
		return m_pEntity->Position();
	}

	void Destroy() {
		m_pEntity->Destroy();
		m_shadow.Destroy();
	}

	bool IsActive() {
		return active;
	}

private:
	Shadow m_shadow;

	//�G���e�B�e�B�ւ̃|�C���^.
	Entity::Entity* m_pEntity;
	//�A�C�e��
	int itemId = -1;
	float time = 0.0f;

	bool active = true;

	float y_Rot = 0.0f;

	RaceScene* m_pRaceScene;
};