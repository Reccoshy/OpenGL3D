#include "ItemGetEffect.h"
#include "../../../GameEngine.h"

/*
初期化処理.

@param	pos		出現位置.
@param	color	色.
@param	emitNum	出現数.
*/
bool CSparcleEffect::Init(glm::vec3 pos, glm::vec4 color, int emitNum)
{
	GameEngine& game = GameEngine::Instance();
	
	m_potition.resize(emitNum);
	m_velocity.resize(emitNum);
	m_scales.resize(emitNum);

	m_emitNum = emitNum;

	for (int i = 0; i < emitNum; i++) {

		m_potition[i] = pos;

		m_velocity[i] = glm::vec3(
			game.GetRandomFloat(m_randMaxVel, m_randMinVel),
			game.GetRandomFloat(m_randMaxVel, 0.0f),
			game.GetRandomFloat(m_randMaxVel, m_randMinVel));
		
		m_scales[i] = game.GetRandomFloat(m_randMaxScale, m_randMinScale);
	}

	m_color = color;
	m_active = true;

	return true;
}

/*
更新処理.

@param	delta	
*/
void CSparcleEffect::Update(float delta)
{
	for (int i = 0; i < m_emitNum; i++) {
		m_potition[i] += m_velocity[i] * delta;
	}

	m_time += delta;
	
	if (m_time > m_lifeTime) {
		m_active = false;
	}
}

/*
有向グラフの取得.
*/
bool CSparcleEffect::IsActive()
{
	return m_active;
}

/*
描画処理.
*/
void CSparcleEffect::Draw()
{
	GameEngine& game = GameEngine::Instance();

	for (int i = 0; i < m_emitNum; i++) {
		game.AddEffect(m_potition[i], glm::vec2(m_scales[i]), m_color, "res/Texture/itemGetSparkle.dds");
	}
}