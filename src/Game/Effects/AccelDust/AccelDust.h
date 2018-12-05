#pragma once
#include <glm/glm.hpp>

//�A�N�Z���_�X�g.
class AccelDust 
{
public:

	bool Init(glm::vec3 pos);

	void Update(float dt);

	void Draw();

	bool ActiveCheck() { return m_active; }

private:
	//�����T�C�Y.
	float m_scale = 10.0f;
	//�ŏI�T�C�Y.
	float m_endScale = 15.0f;
	//1�b�Ԃł̃T�C�Y�ύX��.
	float m_scaleMultiplier = 10.0f;
	//�z�u�ʒu.
	glm::vec3 m_position;
	//�F.
	glm::vec4 m_color = glm::vec4(0.0f, 0.5f, 0.5f, 0.5f);
	//�L���t���O.
	bool m_active = false;
};
