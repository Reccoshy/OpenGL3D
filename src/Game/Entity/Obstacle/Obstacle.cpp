#include "Obstacle.h"
#include "../../../GameEngine.h"

/*
障害物を配置する.

@param	障害物の配置する場所.
@param	障害物の半径および大きさ.
*/
void Obstacle::Init(glm::vec3 pos, float radius)
{
	GameEngine& game = GameEngine::Instance();
	m_pEntity = game.AddEntity(0, glm::vec3(0, 1, 50), "Cube", "res/Model/Toroid.bmp", nullptr);

	m_pEntity->Position(pos);

	m_pEntity->Color(glm::vec4(0, 0, 0, 1));

	this->m_colRadius = radius;

	m_pEntity->Scale(glm::vec3(radius * 0.008));

	return;
}