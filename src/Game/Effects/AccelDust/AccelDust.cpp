#include "AccelDust.h"
#include "../../../GameEngine.h"

/*
����������.

@param	pos	�G�t�F�N�g�̏o���ꏊ.
*/
bool AccelDust::Init(glm::vec3 pos)
{
	m_position = pos;
	m_active = true;

	return true;
}

/*
�X�V����.

@param	dt	1�t���[���̍X�V����.
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
�`�揈��.
*/
void AccelDust::Draw()
{
	GameEngine& game = GameEngine::Instance();

	game.AddEffect(m_position, glm::vec2(m_scale), m_color, "res/Texture/itemGetSparkle.dds");
}
