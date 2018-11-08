#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "../../Entity/Player/Player.h"

class CPlayerCharacter;

class MiniMap
{
public:
	bool Init(glm::vec2 pos, std::string filename);

	void Update(std::vector<CPlayerCharacter*> players, int playerIndex);

private:
	glm::vec2 m_mapOrigin;

	char m_mapFilename[100] = " ";
};