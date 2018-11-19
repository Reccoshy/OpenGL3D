#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../../../Entity.h"
#include "../../Utilities/CommandSelect/CommandSelect.h"
#include "../../Utilities/UITexture/BlackFilter/BlackFilter.h"
#pragma warning(disable:4996)

enum class ShowingMenu{
	PlayerSelect = 0,
	StageSelect = 10,
	RideSelect = 20,
	ReadyCheck = 20,
};

enum class FromStageSelectNextScene {
	NONE = -1,
	ToRaceScene = 0,
	ToTitleScene = 1,
};

class StageSelectScene
{
public:
	explicit StageSelectScene() = default;
	void operator()(double delta);

private:

	bool isInitialized = false;

	CommandSelect m_stageSelect;

	CommandSelect m_playerNum;
	CommandSelect m_lapSelect;

	enum MenuIdex {
		PlayerNumSelect = 0,
		LapNumSelect = 1,
	}m_playerSelecrMenuIndex;

	ShowingMenu m_menu = ShowingMenu::PlayerSelect;

	float m_bestLap = 0.0f;

	float m_time = 0.0f;

	BlackFilter blackFilter;

	bool m_activeInput = false;

	FromStageSelectNextScene m_nextScene = FromStageSelectNextScene::NONE;

	char stageName[100];
	char itemTexfile[100];
	char stageDataTexfile[100];
	char checkPointTexfile[100];
	char scoreFile[100];
	char texFile[100];

	float m_uIAdder = 0.0f;

private:
	bool Init();

	void Update(float delta);

	void EndFunc();

	bool LoadStageData();
	bool LoadHighScore();
	void InputFunc();
	void ShowTextUI(float delta);

	void SceneChanger(float delta);
};