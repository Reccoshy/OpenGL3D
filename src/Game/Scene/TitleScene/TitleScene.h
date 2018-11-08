#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../../../Entity.h"
#include "../../Utilities/CommandSelect/CommandSelect.h"

enum class FromTitleNextScene {
	None = -1,
	ToStageSelect = 0,
	EndGame = 1,
};

class TitleScene
{
public:
	explicit TitleScene() = default;
	void operator()(double delta);

private:

	bool isInitialized = false;

	CommandSelect commandSelect;

	float CamY = 0;

	float m_waitTime = 1.0f;
	float m_UIAlpha = 1.0f;

	bool m_sceneChanging = false;
	bool m_activeInputFlag = false;

	Entity::Entity* m_pPlayerCharacter;

	FromTitleNextScene m_nextScene = FromTitleNextScene::None;

private:
	bool Init();

	void Update(float delta);

	void EndFunc();

	void InputFunc();
	void ShowTextUI();

	void SceneChanger(float delta);
};