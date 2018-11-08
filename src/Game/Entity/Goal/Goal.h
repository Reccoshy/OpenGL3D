#pragma once

#include <glm/glm.hpp>
#include "../../../Entity.h"

//四角のコリジョン
struct Collision {
	glm::vec3 min;
	glm::vec3 max;
};

//目的地を
class Goal {
public:
	bool Init(glm::vec3 pos, glm::vec3 min = glm::vec3(-10, -10, -10), glm::vec3 max = glm::vec3(10, 100, 10));

	bool CollisionCheck(glm::vec3 pos, float radius);

	bool ToggleVisible(bool b);

	glm::vec3 Position() { return m_pEntity->Position(); }

	void Destroy() {
		m_pEntity->Destroy();
	}

private:

	glm::vec3 CalcClosestPoint(glm::vec3 pos, float r);

private:
	//エンティティへのポインタ.
	Entity::Entity* m_pEntity;
	//この目的地のインデックス(0はゴール地点).
	int m_checkPointIndex = -1;
	//初期化されたか.
	bool m_initialized = false;
	//ゴールの位置.
	glm::vec3 m_position = glm::vec3(0, 0, 0);
	//ゴールを判定する位置.
	Collision m_collision = { glm::vec3(-10, -10, -10), glm::vec3(10, 100, 10) };
};