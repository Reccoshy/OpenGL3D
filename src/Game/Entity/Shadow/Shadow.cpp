#include "Shadow.h"

/*
初期化処理.

@param	pos		影を表示する位置.
@param	scale	影の拡大率.
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
更新処理.

@param	pos	更新後の影の位置.
*/
void Shadow::Update(glm::vec3 pos)
{
	this->m_pEntity->Position(glm::vec3(pos.x, -0.9, pos.z));
}

/*
影の表示・非表示を切り替える.

@param	isVisible	true = 影を表示.
					false= 影を非表示.
*/
void Shadow::ChangeVisible(bool isVisible)
{
	this->m_pEntity->ToggleVisibility(isVisible);
}

/*
影を消すための処理.
*/
void Shadow::Destroy()
{
	this->m_pEntity->Destroy();
}