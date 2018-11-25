#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../../../Entity.h"
#include "../../Utilities/CommandSelect/CommandSelect.h"
#include "../../Utilities/UITexture/BlackFilter/BlackFilter.h"

class TitleScene
{
public:
	explicit TitleScene() = default;
	void operator()(double delta);

private:

	bool isInitialized = false;

	CommandSelect commandSelect;

	float CamY = 0;

	BlackFilter blackFilter;

	Entity::Entity* m_pPlayerCharacter;

private:
	bool Init();

	void Update(float delta);

	void EndFunc();

	void InputFunc();
	void ShowTextUI();

	void SceneChanger(float delta);
};