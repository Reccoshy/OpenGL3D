#pragma once
#include <glm/glm.hpp>
#include "../../../../Entity.h"

class RaceScene;

//ダメージを含む爆風.
class Explosion {
public:
	bool Init(glm::vec3 pos, RaceScene* p);

	void Update(float delta);

	bool CollisionCheck(glm::vec3 pos, float radius);

	glm::vec3 Position()
	{
		return m_pEntity->Position();
	}

	void Destroy() {
		m_pEntity->Destroy();
	}

	bool IsActive() {
		return this->active;
	}

private:
	
	RaceScene* m_pRaceScene = nullptr;
	//エンティティのポインタ.
	Entity::Entity* m_pEntity;
	//有効かどうか.
	bool active = false;
	//破壊判定のある大きさ.
	float m_radius = 35.0f;
	//爆発している時間.
	float explodeTime = 1.0f;
	//爆発のサイズが最高になる時間.
	float peakTime = 0.9f;
	//爆発の最大サイズ.
	float scale = 0.01f;
};