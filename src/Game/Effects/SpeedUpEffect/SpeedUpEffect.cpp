#include "SpeedUpEffect.h"
#include "../../../GameEngine.h"

bool SpeedUpEffect::Init(glm::vec3 pos, glm::vec4 col, glm::vec3 vel)
{
	m_position = pos;
	
	m_color = col;
	m_velocity = vel;

	m_scale = glm::vec2(20);

	m_active = true;

	return true;
}

void SpeedUpEffect::Update(float delta)
{
	m_position += delta * m_velocity;
	m_position.y += delta;

	this->m_time += delta;

	if (m_time > m_lifeTime) {
		m_active = false;
	}
}

void SpeedUpEffect::Draw()
{
	GameEngine& game = GameEngine::Instance();

	game.AddEffect(m_position, m_scale, m_color, "res/Texture/itemGetSparkle.dds");
}

bool SpeedUpEffect::IsActive()
{
	return m_active;
}
