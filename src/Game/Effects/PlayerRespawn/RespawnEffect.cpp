#include "RespawnEffect.h"
#include "../../../GameEngine.h"

bool RespawnEffect::Init(glm::vec3 pos, float time)
{
	m_position = pos;
	m_lifeTime = time;
	m_active = true;

	return true;
}

void RespawnEffect::Update(float delta)
{
	this->m_time += delta;

	this->m_currentAnimTime += delta;
	if (m_currentAnimTime > m_animaChangeTime) {
		
		m_currentAnimTime -= m_animaChangeTime;
		m_animeNum++;
		if (m_animeNum > m_lastAnimeNum) {
			m_animeNum = 1;
		}
	}

	if (m_time > m_lifeTime) {
		m_active = false;
	}
}

void RespawnEffect::Draw()
{
	GameEngine& game = GameEngine::Instance();

	char str[32];

	snprintf(str, 32, "res/Texture/respawn%d.dds", m_animeNum);
	game.AddEffect(m_position + glm::vec3(0, m_lifeTime - m_time, 0) * 2.0f, glm::vec2(m_time * 100), glm::vec4(1, 1, 1, 1), str);
}
