#pragma once

#include "../../../../Entity.h"
#include "../../Shadow/Shadow.h"
#include <vector>

class CPlayerCharacter;
class RaceScene;

//レースアイテムミサイル.
class Missile {
public:
	
	bool Init(glm::vec3 pos, float rot, RaceScene* m_pScene, bool aiming = false);


	void Update(float delta, std::vector<CPlayerCharacter*> player);


	bool CollisionCheck(glm::vec3 pos, float radius);


	void DetectPlayersAndChase(float delta, std::vector<CPlayerCharacter*> players);

	void SpawnEffects();

	glm::vec3 Position()
	{
		return m_pEntity->Position();
	}

	bool IsActive() { return m_isActive; }

	void Destroy();

private:
	//影
	Shadow m_shadow;

	//使用しているエンティティのポインタ.
	Entity::Entity* m_pEntity = nullptr;
	
	//レース中のシーン.
	RaceScene* m_pRaceScene = nullptr;


	float m_radius = 1.0f;
	//向いている方向.
	float m_Yrot = 0.0f;

	//1秒で到達する速さ.
	float const m_velocity = 200.0f;
	//1秒で回転する.
	float const m_turnRate = 20.0f;

	//アイテムが有効か.
	bool m_isActive = true;

	//追尾するかどうか.
	bool m_Aiming = true;
};