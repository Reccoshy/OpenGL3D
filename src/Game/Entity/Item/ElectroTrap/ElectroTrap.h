#pragma once

#include "../../../../Entity.h"
#include "../../Shadow/Shadow.h"

class RaceScene;

//�A�C�e�� �ݒu����g���b�v
class ElectroTrap {
public:
	bool init(glm::vec3 pos, RaceScene* p);

	bool CollisionCheck(glm::vec3 pos, float radius);

	glm::vec3 Position()
	{
		return m_pEntity->Position();
	}

	bool IsActive()
	{
		return isActive;
	}

	void Destroy();

private:
	Shadow m_shadow;

	RaceScene* m_pRaceScene = nullptr;

	//�g���b�v�̃G���e�B�e�B�ւ̃|�C���^.
	Entity::Entity* m_pEntity = nullptr;

	//�g���b�v���L����.
	bool isActive = true;
};