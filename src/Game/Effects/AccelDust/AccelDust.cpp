#include "AccelDust.h"
#include "../../../GameEngine.h"

/*
初期化処理.

@param	pos	エフェクトの出現場所.
*/
bool AccelDust::Init(glm::vec3 pos)
{
	m_position = pos;
	m_active = true;

	return true;
}

/*
更新処理.

@param	dt	1フレームの更新処理.
*/
void AccelDust::Update(float dt)
{
	if (m_active) {

		m_scale += dt * m_scaleMultiplier;

		if (m_scale > m_endScale) {
			this->m_active = false;
		}
	}
}

/*
描画処理.
*/
void AccelDust::Draw()
{
	GameEngine& game = GameEngine::Instance();

	game.AddEffect(m_position, glm::vec2(m_scale), m_color, "res/Texture/itemGetSparkle.dds");
}
