#include "MiniMap.h"
#include "../../../GameEngine.h"

/*
初期化処理

@param	pos			描画を設定する場所の設定.
@param	filename	描画するマップの画像名.

@return 初期化成功
*/
bool MiniMap::Init(glm::vec2 pos, std::string filename)
{
	this->m_mapOrigin = pos;

	snprintf(m_mapFilename, 100, "%s", filename);

	return true;
}

/*
更新処理.

@param players		シーン上にいるプレイヤー全てのポインタ.
@param playerIndex	マップ上のメインとするプレイヤーのインデックス.
*/
void MiniMap::Update(std::vector<CPlayerCharacter*> players, int playerIndex)
{
	GameEngine& game = GameEngine::Instance();
	glm::vec2 windowSize = game.GetWindowSize();

	glm::vec2 mapPos = glm::vec2(/*700*/windowSize.x -500.0f, 100);

	game.ImageScale(glm::vec2(1));
	game.ImageColor(glm::vec4(1));
	game.AddImage(mapPos, m_mapFilename, playerIndex);

	glm::vec4 colors[] = {
		glm::vec4(1, 0, 0, 1),
		glm::vec4(0, 0, 1, 1),
		glm::vec4(1, 0.5, 0, 1),
		glm::vec4(0, 1, 0, 1),
		glm::vec4(0.2, 0.2, 0.2, 1)
	};

	//マップ上のプレイヤーの位置を表示
	for (int j = 0; j < players.size(); j++) {

		if (j == playerIndex) {
			continue;
		}

		glm::vec2 pos = glm::vec2(players[j]->Position().z, -players[j]->Position().x);
		glm::vec2 mapOffset = glm::vec2(160, 180);

		pos *= 0.25;
		game.ImageScale(glm::vec2(0.75f));
		game.ImageColor(colors[j]);
		game.AddImage(mapPos + pos + mapOffset, "res/Texture/MiniMap/mapPlayerDotUi.dds", playerIndex);
	}

	//自分の位置を一番最後に描画.
	glm::vec2 pos = glm::vec2(players[playerIndex]->Position().z, -players[playerIndex]->Position().x);
	glm::vec2 mapOffset = glm::vec2(160, 180);

	pos *= 0.25;
	game.ImageColor(colors[playerIndex]);
	game.ImageScale(glm::vec2(0.75f));
	game.AddImage(mapPos + pos + mapOffset, "res/Texture/MiniMap/mapPlayerDotUi.dds", playerIndex);
}