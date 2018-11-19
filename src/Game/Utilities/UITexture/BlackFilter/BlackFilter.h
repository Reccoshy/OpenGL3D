#pragma once

#include <glm/glm.hpp>

enum NEXT_SCENE {
	None = 0,
	Title = 1,
	StageSelect = 2,
	InRace = 3,
	Result = 4,
	EndGame = 5,
};

class BlackFilter {
public:

	bool Init();

	void Update(float delta);

	void SetNextScene(NEXT_SCENE scene);

	NEXT_SCENE NextScene();

	bool IsInAction();

private:

	void Draw();

private:

	NEXT_SCENE m_nextScene = NEXT_SCENE::None;

	bool m_sceneOpening = true;
	bool m_sceneChanging = false;

	bool m_changeNow = false;

	float m_UIalpha = 1.0f;
	float m_waitTime = 1.0f;
};