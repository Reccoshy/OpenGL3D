#include "AccelDust.h"
#include "../../../GameEngine.h"

bool AccelDust::Init(glm::vec3 pos)
{
	GameEngine& game = GameEngine::Instance();

	game.AddEffect(pos, glm::vec2(m_scale), m_color, "res/Texture/MiniMap/mapPlayerDotUi.dds");

	m_position = pos;

	m_active = true;

	return true;
}

void AccelDust::Update(float dt)
{
	if (m_active) {

		m_scale += dt * m_scaleMultiplier;

		if (m_scale > m_endScale) {
			this->m_active = false;
		}
	}
}

void AccelDust::Draw()
{
	GameEngine& game = GameEngine::Instance();

	game.AddEffect(m_position, glm::vec2(m_scale), m_color, "res/Texture/MiniMap/mapPlayerDotUi.dds");
}
