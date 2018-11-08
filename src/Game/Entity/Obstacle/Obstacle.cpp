#include "Obstacle.h"
#include "../../../GameEngine.h"

void Obstacle::Init(glm::vec3 pos, float radius)
{
	GameEngine& game = GameEngine::Instance();
	m_pEntity = game.AddEntity(0, glm::vec3(0, 1, 50), "Cube", "res/Model/Toroid.bmp", nullptr);

	m_pEntity->Position(pos);

	m_pEntity->Color(glm::vec4(0, 0, 0, 1));

	this->m_colRadius = radius;

	m_pEntity->Scale(glm::vec3(radius */* 1.4*/  0.008));

	return;
}