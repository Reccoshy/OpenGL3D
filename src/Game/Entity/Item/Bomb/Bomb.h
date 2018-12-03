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

	/*
	エンティティの位置の取得.

	@return	エンティティの位置.
	*/
	glm::vec3 Position() {
		return m_pEntity->Position();
	}

	/*
	有効フラグ無効後の破壊処理.
	*/
	void Destroy() {
		m_pEntity->Destroy();
		m_shadow.Destroy();
	}

	/*
	有効フラグのチェック.

	@return 有効フラグ.
	*/
	bool IsActive() {
		return m_active;
	}

private:

	Shadow m_shadow;
	//レースシーンのポインタ.
	RaceScene* m_pRaceScene;
	//エンティティへのポインタ.
	Entity::Entity* m_pEntity;
	//有効かどうか.
	bool m_active = true;
	//移動速度(後方にこの速度で移動)
	float m_velocity = 80.0f;
	//正面の角度.
	float yRot = 0.0f;
	//爆発するまでの時間.
	float explodeTime = 0.5f;
};