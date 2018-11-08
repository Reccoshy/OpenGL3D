#pragma once
#include <glm/glm.hpp>
#include "../../../BufferObject.h"
#include "../../../Shader.h"
#include <glew.h>
#include <vector>
#include "../../../Texture.h"

class GameEngine;
struct CameraData;

class Effect {
public:
	void Init(glm::vec2 size);

	void AddEffect(glm::vec3 pos, glm::vec2 s, glm::vec4 color, const char* imageName);

	void Draw(int index);

	void EndClear();

private:

	int width = 0;
	int height = 0;

	int m_effectNum = 0;

	std::vector<glm::vec3> positions;

	std::vector<glm::vec2> scale;

	std::vector<TexturePtr> textures;

	std::vector<GLuint> effectsVbo;
	std::vector<GLuint> effectsVao;

	Shader::ProgramPtr program = 0;
};