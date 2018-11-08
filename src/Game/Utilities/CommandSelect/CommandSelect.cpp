#include "CommandSelect.h"
#include "../../../GameEngine.h"
#include "../../../res/Sound/Sound.h"

void CommandSelect::InitCommand(int Max, int initialIndex)
{
	this->MaxIndex = Max - 1;

	this->index = initialIndex;
}

void CommandSelect::IncreaseIndex()
{
	GameEngine& game = GameEngine::Instance();

	index++;
	if (index > MaxIndex) {
		index = 0;
	}

	game.PlayAudio(SEUI, CRI_SOUND_CURSOR);
}

void CommandSelect::DecreaseIndex()
{
	GameEngine& game = GameEngine::Instance();

	index--;

	if (index < 0) {
		index = MaxIndex;
	}

	game.PlayAudio(SEUI, CRI_SOUND_CURSOR);
}