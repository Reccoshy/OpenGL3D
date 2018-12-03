#pragma once

#include "../../../Entity.h"

//�A�C�e���┚���������������̃G�t�F�N�g.
class EffectDestroy {
public:

	bool Init(glm::vec3 pos);

	void Update(float delta);

	bool IsActive() { return m_active; }

	void Destroy();

private:

	void ChangeColor();

private:
	//������������܂ł̎���.
	float m_lifeTime = 0.5f;
	//�����̃G���e�B�e�B�ւ̃|�C���^.
	Entity::Entity* m_pEntity;
	//������\�����Ă��邩.
	bool m_active = true;
};