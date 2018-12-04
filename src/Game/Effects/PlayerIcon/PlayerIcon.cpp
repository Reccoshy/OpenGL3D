#include "PlayerIcon.h"
#include "../../../GameEngine.h"

bool PlayerIcon::Init(int index, float height, glm::vec3 pos)
{
	m_position = pos;
	m_index = index;
	m_height = height;

	return false;
}

void PlayerIcon::Update(glm::vec3 pos)
{
	m_position = pos;
}

void PlayerIcon::Draw()
{
	GameEngine& game = GameEngine::Instance();

	glm::vec3 p = m_position + glm::vec3(0, m_height, 0);

	glm::vec4 color[4]
	{
		glm::vec4(1,0,0,1),
		glm::vec4(0,0,1,1),
		glm::vec4(1,1,0,1),
		glm::vec4(0,1,0,1),
	};

	game.AddEffect(p, glm::vec2(50), color[m_index], "res/Texture/SpeedMeter.dds");
}