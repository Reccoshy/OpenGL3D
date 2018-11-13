#include "ItemGetEffect.h"
#include "../../../GameEngine.h"

bool CitemGetEffect::Init(glm::vec3 pos, glm::vec4 color)
{
	GameEngine& game = GameEngine::Instance();
	
	for (int i = 0; i < 5; i++) {

		m_potition[i] = pos;

		m_velocity[i] = glm::vec3(
			game.GetRandomFloat(m_randMaxVel, m_randMinVel),
			game.GetRandomFloat(m_randMaxVel, 0.0f),
			game.GetRandomFloat(m_randMaxVel, m_randMinVel));
		
		m_scale[i] = game.GetRandomFloat(m_randMaxScale, m_randMinScale);
	}

	m_color = color;

	m_active = true;

	return true;
}

void CitemGetEffect::Update(float delta)
{
	for (int i = 0; i < 5; i++) {
		m_potition[i] += m_velocity[i] * delta;
	}

	m_time += delta;
	
	if (m_time > m_lifeTime) {
		m_active = false;
	}
}

void CitemGetEffect::Draw()
{
	GameEngine& game = GameEngine::Instance();

	for (int i = 0; i < 5; i++) {
		game.AddEffect(m_potition[i], glm::vec2(m_scale[i]), m_color, "res/Texture/itemGetSparkle.dds");
	}
}

bool CitemGetEffect::IsActive()
{
	return m_active;
}
