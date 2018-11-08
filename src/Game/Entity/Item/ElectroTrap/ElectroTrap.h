#pragma once

#include "../../../../Entity.h"
#include "../../Shadow/Shadow.h"

class RaceScene;

//アイテム 設置するトラップ
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

	//トラップのエンティティへのポインタ.
	Entity::Entity* m_pEntity = nullptr;

	//トラップが有効か.
	bool isActive = true;
};