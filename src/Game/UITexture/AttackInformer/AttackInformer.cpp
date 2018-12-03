#include "AttackInformer.h"
#include "../../../GameEngine.h"

bool AttackInformer::Init(float distance)
{
	m_informDistance = distance;

	m_positon.y = GameEngine::Instance().GetWindowSize().y * 0.95;

	return true;
}

void AttackInformer::Update(int index, glm::vec3 origin, glm::vec3 items, float yRot)
{
	float radian = atan2(origin.x - items.x, origin.z - items.z);
	float degree = glm::degrees(radian);

	float check = degree - yRot;

	check += 180;

	if (check < 0) {
		check += 360;
	}
	

	glm::vec2 winSize = GameEngine::Instance().GetWindowSize();

	float place = check / 360;

	place = winSize.x * place;

	m_positon.x = place;

	if (index == 0) {

		printf("%f %f\n", check, yRot, m_positon.x);
	}

	this->Draw(index);
}

void AttackInformer::Draw(int index)
{
	GameEngine& game = GameEngine::Instance();

	game.ImageScale(glm::vec2(1));

	game.AddImage(m_positon, "res/Texture/itemGetSparkle.dds", index);
}
