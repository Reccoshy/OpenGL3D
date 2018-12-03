#pragma once
#include "../../../../Entity.h"
#include "../../Shadow/Shadow.h"

class RaceScene;


class CNuke {
public:
	bool Init(glm::vec3 pos, float rot, RaceScene* p);

	void Update(float delta);

	/*
	エンティティの場所を取得.

	@return	エンティティの場所.
	*/
	glm::vec3 Positioin()
	{
		return m_pEntity->Position();
	}

	/*
	終了時の破壊処理.
	*/
	void Destroy() {
		m_pEntity->Destroy();
		m_shadow.Destroy();
	}

	/*
	有効フラグの確認.

	@return	有効フラグ.
	*/
	bool IsActive() {
		return m_isActive;
	}

private:
	//影
	Shadow m_shadow;
	//自身のエンティティ.
	Entity::Entity* m_pEntity = nullptr;
	//向いている方向.
	float m_Yrot = 0.0f;
	//飛ばすスピード.
	float const m_velocity = 200.0f;
	//現在の高さ.
	float const m_height = 0.0f;

	//発生からの経過時間.
	float m_time = 0.0f;
	//落下を開始する時間.
	float m_startFallTime = 0.5f;
	//レースシーンのポインタ.
	RaceScene* m_pRaceScene;
	//有効フラグ.
	bool m_isActive = true;
};