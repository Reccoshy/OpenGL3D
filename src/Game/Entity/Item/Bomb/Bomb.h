#pragma once

#include <glm/glm.hpp>
#include "../../../../Entity.h"
#include "../../Shadow/Shadow.h"

class RaceScene;

//後方の投げる爆弾.
class Bomb {
public:
	bool Init(glm::vec3 pos, float yRot, RaceScene* rs);

	void Update(float delta);

	glm::vec3 Position() {
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
	//レースシーンのポインタ.
	RaceScene* m_pRaceScene;
	//エンティティへのポインタ.
	Entity::Entity* m_pEntity;
	//有効かどうか.
	bool active = true;
	//移動速度(後方にこの速度で移動)
	float m_velocity = 80.0f;
	//正面の角度.
	float yRot = 0.0f;
	//爆発するまでの時間.
	float explodeTime = 0.5f;
};