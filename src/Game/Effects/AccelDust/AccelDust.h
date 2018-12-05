#pragma once
#include <glm/glm.hpp>

//アクセルダスト.
class AccelDust 
{
public:

	bool Init(glm::vec3 pos);

	void Update(float dt);

	void Draw();

	bool ActiveCheck() { return m_active; }

private:
	//初期サイズ.
	float m_scale = 10.0f;
	//最終サイズ.
	float m_endScale = 15.0f;
	//1秒間でのサイズ変更率.
	float m_scaleMultiplier = 10.0f;
	//配置位置.
	glm::vec3 m_position;
	//色.
	glm::vec4 m_color = glm::vec4(0.0f, 0.5f, 0.5f, 0.5f);
	//有効フラグ.
	bool m_active = false;
};
