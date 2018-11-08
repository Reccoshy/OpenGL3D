#include "Shadow.h"

bool Shadow::init(glm::vec3 pos, float scale)
{
	GameEngine& game = GameEngine::Instance();

	this->m_pEntity = GameEngine::Instance().AddEntity(0, pos, "Shadow", "res/Model/Toroid.bmp", nullptr);

	this->m_pEntity->Color(glm::vec4(0, 0, 0, 1));

	this->m_pEntity->Position(glm::vec3(pos.x, -0.9, pos.z));
		
	this->m_pEntity->Scale(glm::vec3(scale));

	return this;
}

void Shadow::Update(glm::vec3 pos)
{
	this->m_pEntity->Position(glm::vec3(pos.x, -0.9, pos.z));
}

void Shadow::ChangeVisible(bool isVisible)
{
	this->m_pEntity->ToggleVisibility(isVisible);
}

void Shadow::Destroy()
{
	this->m_pEntity->Destroy();
}