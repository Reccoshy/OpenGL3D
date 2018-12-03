#pragma once

#include "../../../Entity.h"

//アイテムや爆発が発生した時のエフェクト.
class EffectDestroy {
public:

	bool Init(glm::vec3 pos);

	void Update(float delta);

	bool IsActive() { return m_active; }

	void Destroy();

private:

	void ChangeColor();

private:
	//爆発が消えるまでの時間.
	float m_lifeTime = 0.5f;
	//爆発のエンティティへのポインタ.
	Entity::Entity* m_pEntity;
	//爆発を表示しているか.
	bool m_active = true;
};