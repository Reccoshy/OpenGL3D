#pragma once

#include <glm/glm.hpp>

#include "../../../Entity.h"

//障害物クラス
class Obstacle {
public:
	
	void Init(glm::vec3 pos, float radius);

	/*
	半径を取得する.

	@return 半径を取得.
	*/
	float Radius() { return m_colRadius; }
	
	/*
	エンティティの場所を取得する.
	
	@return	エンティティの場所.
	*/
	glm::vec3 Position() { return m_pEntity->Position(); }

	/*
	エンティティの取得.

	@return	エンティティのポインタ.
	*/
	Entity::Entity* Entity() { return m_pEntity; }

private:
	//障害物のエンティティ.
	Entity::Entity* m_pEntity = nullptr;
	//あたり判定用の半径.
	float m_colRadius = 0.0f;
};