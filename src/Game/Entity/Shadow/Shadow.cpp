#include "Shadow.h"

/*
‰Šú‰»ˆ—.

@param	pos		‰e‚ð•\Ž¦‚·‚éˆÊ’u.
@param	scale	‰e‚ÌŠg‘å—¦.
*/
bool Shadow::init(glm::vec3 pos, float scale)
{
	GameEngine& game = GameEngine::Instance();

	this->m_pEntity = GameEngine::Instance().AddEntity(0, pos, "Shadow", "res/Model/Toroid.bmp", nullptr);

	this->m_pEntity->Color(glm::vec4(0, 0, 0, 1));

	this->m_pEntity->Position(glm::vec3(pos.x, -0.9, pos.z));
		
	this->m_pEntity->Scale(glm::vec3(scale));

	return this;
}

/*
XVˆ—.

@param	pos	XVŒã‚Ì‰e‚ÌˆÊ’u.
*/
void Shadow::Update(glm::vec3 pos)
{
	this->m_pEntity->Position(glm::vec3(pos.x, -0.9, pos.z));
}

/*
‰e‚Ì•\Ž¦E”ñ•\Ž¦‚ðØ‚è‘Ö‚¦‚é.

@param	isVisible	true = ‰e‚ð•\Ž¦.
					false= ‰e‚ð”ñ•\Ž¦.
*/
void Shadow::ChangeVisible(bool isVisible)
{
	this->m_pEntity->ToggleVisibility(isVisible);
}

/*
‰e‚ðÁ‚·‚½‚ß‚Ìˆ—.
*/
void Shadow::Destroy()
{
	this->m_pEntity->Destroy();
}