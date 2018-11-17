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

	if (m_time > m_lifeTime) {
		m_active = false;
	}
}

void RespawnEffect::Draw()
{
	GameEngine& game = GameEngine::Instance();

	game.AddEffect(m_position, glm::vec2(100), glm::vec4(1, 1, 1, 1), "res/Texture/Respawn.dds");
}
