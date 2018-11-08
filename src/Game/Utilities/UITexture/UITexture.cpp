/*
@file UITexture.cpp
*/
#include "UITexture.h"
#include "../../../GameEngine.h"
#include <memory>
#include <iostream>
#include <stdio.h>

/*
�t�H���g�`��@�\���i�[���閼�O���.
*/
namespace UITexture {

	/*
	�t�H���g�p���_�f�[�^�^.
	*/
	struct Vertex
	{
		glm::vec2 position;
		glm::u16vec2 uv;
		glm::u8vec4 color;
	};

	/*
	�t�H���g�`��I�u�W�F�N�g������������.

	@param maxChar	�ő�`�敶����(0~16384�̊Ԃ̐��l).
	@param screen	�`���X�N���[���̑傫��(�s�N�Z���P��).
	@param fontSize	��ʂɕ\�������t�H���g�T�C�Y(�s�N�Z���P��).

	@retval true	����������.
	@retval false	���������s.
	*/
	bool Renderer::Init(size_t maxChar, const glm::vec2& screen, glm::f32 fontSize)
	{
		if (maxChar > (USHRT_MAX + 1) / 4) {
			std::cerr << "WARNING: " << maxChar << "�͐ݒ�\�ȍő啶�������z���Ă��܂�" << std::endl;
			maxChar = (USHRT_MAX + 1) / 4;
		}
		vboCapacity = static_cast<GLsizei>(4 * maxChar);
		vbo.Init(GL_ARRAY_BUFFER, sizeof(Vertex) * vboCapacity, nullptr, GL_STREAM_DRAW);
		{
			std::vector<GLushort> tmp;
			tmp.resize(maxChar * 6);
			GLushort* p = tmp.data();
			for (GLushort i = 0; i < maxChar * 4; i += 4) {
				for (GLushort n : { 0, 1, 2, 2, 3, 0 }) {
					*(p++) = i + n;
				}
			}
			ibo.Init(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6 * maxChar, tmp.data(),
				GL_STATIC_DRAW);
		}
		vao.Init(vbo.Id(), ibo.Id());
		vao.Bind();
		vao.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			offsetof(Vertex, position));
		vao.VertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex),
			offsetof(Vertex, uv));
		vao.VertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
			offsetof(Vertex, color));
		vao.Unbind();

		progFont = Shader::Program::Create("Res/UITexture.vert", "Res/UITexture.frag");
		if (!progFont) {
			return false;
		}

		windowSize = screen;

		normalFontSize = fontSize;
		pixelSizeInClipCoord = 2.0f / screen;
		return true;
	}

	/*
	�t�H���g�t�@�C����ǂݍ���.

	@param filename	�t�@�C����.

	@retval true	�ǂݍ��ݐ���.
	@retval false	�ǂݍ��ݎ��s.
	*/
	bool Renderer::LoadFromFile(const char* filename)
	{
		const std::unique_ptr<FILE, decltype(&fclose)> fp(fopen(filename, "r"), fclose);
		if (!fp) {
			std::cerr << "ERROR: '" << filename << "'�̃I�[�v���Ɏ��s" << std::endl;
			return false;
		}

		int line = 1; ///< �t�H���g�t�@�C���̏������̍s.
		float size;
		int ret = fscanf(fp.get(),
			"info face=%*s size=%f bold=%*d italic=%*d charset=%*s unicode=%*d"
			" stretchH=%*d smooth=%*d aa=%*d padding=%*d,%*d,%*d,%*d spacing=%*d,%*d", &size);
		if (ret < 1) {
			std::cerr << "ERROR: '" << filename << "'�̓ǂݍ��݂Ɏ��s(line=" << line <<
				")" << std::endl;
			return false;
		}
		baseScale = (normalFontSize / size) * pixelSizeInClipCoord;
		++line;

		glm::vec2 scale;
		ret = fscanf(fp.get(),
			" common lineHeight=%*d base=%*d scaleW=%f scaleH=%f pages=%*d packed=%*d",
			&scale.x, &scale.y);
		if (ret < 2) {
			std::cerr << "ERROR: '" << filename << "'�̓ǂݍ��݂Ɏ��s(line=" << line <<
				")" << std::endl;
			return false;
		}
		const glm::vec2 reciprocalScale(1.0f / scale);
		++line;

		char tex[128];
		ret = fscanf(fp.get(), " page id=%*d file=%127s", tex);
		if (ret < 1) {
			std::cerr << "ERROR: '" << filename << "'�̓ǂݍ��݂Ɏ��s(line=" << line <<
				")" << std::endl;
			return false;
		}
		//texFilename = "Res/";
		//texFilename += std::string(tex + 1, tex + strlen(tex) - 1);
		++line;

		int charCount;
		ret = fscanf(fp.get(), " chars count=%d", &charCount);
		if (ret < 1) {
			std::cerr << "ERROR: '" << filename << "'�̓ǂݍ��݂Ɏ��s(line=" << line <<
				")" << std::endl;
			return false;
		}
		++line;

		fontList.resize(128); // ASCII�t�H���g�����̏ꍇ128��������Ώ\���̂͂�.
		for (int i = 0; i < charCount; ++i) {
			FontInfo font;
			glm::vec2 uv;
			ret = fscanf(fp.get(),
				" char id=%d x=%f y=%f width=%f height=%f xoffset=%f yoffset=%f xadvance=%*d"
				" page=%*d chnl=%*d", &font.id, &uv.x, &uv.y, &font.size.x, &font.size.y,
				&font.offset.x, &font.offset.y);
			if (ret < 8) {
				std::cerr << "ERROR: '" << filename << "'�̓ǂݍ��݂Ɏ��s(line=" << line <<
					")" << std::endl;
				return false;
			}
			font.offset.y *= -1;
			uv.y = scale.y - uv.y - font.size.y;
			font.uv[0] = uv * reciprocalScale * 65535.0f;
			font.uv[1] = (uv + font.size) * reciprocalScale * 65535.0f;
			if (font.id < 128) {
				fontList[font.id] = font;
			}
			++line;
		}
		// �e�N�X�`����ǂݍ���.
		//if (!GameEngine::Instance().LoadTextureFromFile(texFilename.c_str())) {
		//	return false;
		//}
		return true;
	}

	bool Renderer::SetTexture(const glm::vec2& position, const char* str, int CameraIndex, int cameraNum, bool SingleCamWrite)
	{
		if (!GameEngine::Instance().LoadTextureFromFile(str)) {
			return false;
		}
		texFilename.resize(texFilename.size() + 1);
		texFilename[texFilename.size() - 1] = str;

		glm::vec2 camOffset = glm::vec2(0);
		glm::vec2 camScale = glm::vec2(1);
		glm::vec2 posRatio = glm::vec2(1);

		if (!SingleCamWrite) {
			if (cameraNum >= 3) {
				camScale = glm::vec2(0.5f);

				camOffset = glm::vec2(windowSize.x * 0.5 * (CameraIndex % 2), windowSize.y * 0.5 * (CameraIndex / 2));
			}
			else if (cameraNum >= 2) {
				camScale = glm::vec2(1.0f, 0.5f);

				posRatio = glm::vec2(2, 1);
				camOffset = glm::vec2(0, windowSize.y * 0.5 * CameraIndex);
			}
		}

		TexturePtr tex = GameEngine::Instance().GetTexture(texFilename[texFilename.size() - 1].c_str());

		glm::vec2 texSize = glm::vec2(tex->Width() * scale.x, tex->Height() * scale.y);
		const glm::vec2 reciprocalScale(1.0f / scale);

		glm::u16vec2 uv[2];
		uv[0] = glm::vec2(0,0) * reciprocalScale * 65535.0f;
		uv[1] = texSize * reciprocalScale * 65535.0f;


		// ��������_�Ƃ���s�N�Z�����W�n����OpenGL�̃N���b�v���W�n�֕ϊ�.
		glm::vec2 pos = ((position * camScale * posRatio + camOffset) * pixelSizeInClipCoord - 1.0f) * glm::vec2(1.0f, -1.0f);
		Vertex* p = pVBO + vboSize;
		if (vboSize + 4 > vboCapacity) { // VBO�ɋ󂫂��Ȃ���ΏI��.
			return false;
		}
		//FontInfo& font = fontList[0];
		if (tex && texSize.x && texSize.y) { // �\���ł��Ȃ��f�[�^�Ȃ疳������.
			const glm::vec2 size = texSize * baseScale * scale * camScale;
			const glm::vec2 offsetedPos = pos + /*font.offset * */ baseScale * scale * camScale;
			p[0].position = offsetedPos + glm::vec2(0, -size.y);
			p[0].uv = uv[0];//font.uv[0];
			p[0].color = color;
			p[1].position = offsetedPos + glm::vec2(size.x, -size.y);
			p[1].uv = glm::u16vec2(uv[1].x, uv[0].y);
			p[1].color = color;
			p[2].position = offsetedPos + glm::vec2(size.x, 0);
			p[2].uv = uv[1];
			p[2].color = color;
			p[3].position = offsetedPos;
			p[3].uv = glm::u16vec2(uv[0].x, uv[1].y);
			p[3].color = color;
			p += 4;
			vboSize += 4;
		}
	//	pos.x += font.xadvance * baseScale.x * scale.x * camScale.x; // ���̕����̕\���ʒu�ֈړ�.
		return true;
	}

	/*
	�����F��ݒ肷��.

	@param c�����F.
	*/
	void Renderer::Color(const glm::vec4& c)
	{
		color = glm::clamp(c, 0.0f, 1.0f) * 255.0f;
	}

	/*
	�����F���擾����.

	@return �����F.
	*/
	glm::vec4 Renderer::Color() const
	{
		return glm::vec4(color) * (1.0f / 255.0f);
	}

	/*
	VBO���V�X�e���������Ƀ}�b�s���O����.
	*/
	void Renderer::MapBuffer()
	{
		if (pVBO) {
			return;
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo.Id());
		pVBO = static_cast<Vertex*>(glMapBufferRange(GL_ARRAY_BUFFER, 0,
			sizeof(Vertex) * vboCapacity, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		vboSize = 0;
	}

	/*
	VBO�̃}�b�s���O����������.
	*/
	void Renderer::UnmapBuffer()
	{
		if (!pVBO || vboSize == 0) {
			return;
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo.Id());
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		pVBO = nullptr;
	}

	/*
	�t�H���g��`�悷��.
	*/
	void Renderer::Draw()
	{
		if (vboSize == 0) {
			return;
		}
		vao.Bind();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		progFont->UseProgram();
		for (int i = 0; i < texFilename.size(); i++) {
			progFont->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, GameEngine::Instance().GetTexture(texFilename[i].c_str())->Id());
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, reinterpret_cast<const GLint*>(12 * i));
		}
		vao.Unbind();

		this->texFilename.clear();
		this->texFilename.resize(0);
	}
}