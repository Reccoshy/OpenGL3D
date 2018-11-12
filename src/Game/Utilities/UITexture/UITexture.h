/*
@file UITexture.h
*/
#pragma once
#include <glew.h>
#include "../../../BufferObject.h"
#include "../../../Shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace UITexture {

	struct Vertex;

	/*
	ビットマップフォント描画クラス.
	*/
	class Renderer
	{
	public:
		Renderer() = default;
		~Renderer() = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		bool Init(size_t maxImage, const glm::vec2& ss);
		bool SetTexture(const glm::vec2& position, const char* str, int CameraIndex, int cameraNum, bool SingleCamWrite);

		const glm::vec2& GetWindowSize() const { return windowSize; }
		void Scale(const glm::vec2& s) { scale = s; }
		const glm::vec2& Scale() const { return scale; }
		void Color(const glm::vec4& c);
		glm::vec4 Color() const;

		void MapBuffer();
		void UnmapBuffer();
		void Draw();

	private:
		glm::vec2 windowSize;

		BufferObject vbo;
		BufferObject ibo;
		VertexArrayObject vao;
		GLsizei vboCapacity = 0;        ///< VBOに格納可能な最大頂点数.
		std::vector<std::string> texFilename;        ///< フォントテクスチャファイル名.
		Shader::ProgramPtr m_progUITex;    ///< フォント描画用シェーダプログラム.
		glm::vec2 pixelSizeInClipCoord; ///< クリップ座標系における1ピクセルの大きさ.
		glm::f32 normalFontSize;        ///< 拡大率1.0ときに画面に表示されるサイズ(ピクセル単位).
		glm::vec2 baseScale;            ///< フォントをnormalFontSizeで表示するための倍率.

		glm::vec2 scale = glm::vec2(1, 1); ///< フォントを描画するときの拡大率.
		glm::u8vec4 color = glm::u8vec4(255, 255, 255, 255); ///< フォントを描画するときの色.
		GLsizei vboSize = 0;            ///< VBOに格納されている頂点数.
		Vertex* pVBO = nullptr;         ///< VBOへのポインタ.
	};
}