/*
@file GameEngine.h
*/
#pragma once
#include "GLFWEW.h"
#include "UniformBuffer.h"
#include "OffScreenBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Entity.h"
#include "InterfaceBlock.h"
#include "Font.h"
#include "game/Utilities/UITexture/UITexture.h"
#include "game/Utilities/Effects/Effects.h"
#include <glm/glm.hpp>
#include <functional>
#include <random>

enum SoundType {
	BGM = 0,
	SEUI = 1,
	OTHERS = 2,
};

/*
ゲームエンジンクラス.
*/
class GameEngine
{
public:
	typedef std::function<void(double)> UpdateFuncType; ///< ゲーム状態を更新する関数の型.
	/// カメラデータ.
	struct CameraData {
		glm::vec3 position;
		glm::vec3 target;
		glm::vec3 up;
	};

	static GameEngine& Instance();
	bool Init(int w, int h, const char* title, bool fullScreen = false);
	void Run();
	void UpdateFunc(const UpdateFuncType& func);
	const UpdateFuncType& UpdateFunc() const;

	bool LoadTextureFromFile(const char* filename);
	const TexturePtr& GetTexture(const char* filename) const;
	bool LoadCubeMap(std::vector<const char*> faces, const char * name);
	bool LoadMeshFromFile(const char* filename);
	Entity::Entity* AddEntity(int groupId, const glm::vec3& pos, const char* meshName,
		const char* texName, Entity::Entity::UpdateFuncType func, bool isInitialWrite = false,
		const char* shader = nullptr);
	void RemoveEntity(Entity::Entity*);
	void Light(int index, const InterfaceBlock::PointLight& light);
	const InterfaceBlock::PointLight& Light(int index) const;
	void AmbientLight(const glm::vec4& color);
	const glm::vec4& AmbientLight() const;
	void Camera(const CameraData& cam, int index);
	const CameraData& Camera(int index) const;
	
	const GamePad& GetGamePad(int i) const;
	void CollisionHandler(int gid0, int gid1, Entity::CollisionHandlerType handler);
	const Entity::CollisionHandlerType& CollisionHandler(int gid0, int gid1) const;
	void ClearCollisionHandlerList();

	void SetCubeMap(const char * filename);

	bool SetGroundTexture(int textureNum ,const char* filename);
	void RemoveGroundTexture();

	glm::vec2 const GetWindowSize() { return glm::vec2(width, height); }

	bool LoadFontFromFile(const char* filename) {
		return fontRenderer.LoadFromFile(filename);
	}
	bool AddString(const glm::vec2& pos, const char* str, int cameraIndex = 0, bool singleCamWrite = false) {
		return fontRenderer.AddString(pos, str, cameraIndex, CameraNum, singleCamWrite);
	}
	void FontScale(const glm::vec2& scale) { fontRenderer.Scale(scale); }
	void FontColor(const glm::vec4& color) { fontRenderer.Color(color); }

	bool AddImage(const glm::vec2& pos, char* str, int cameraIndex = 0, bool singleCamWrite = false) {
		return uiRenderer.SetTexture(pos, str, cameraIndex, CameraNum, singleCamWrite);
	}
	bool FrontAddImage(const glm::vec2& pos, char* str, int cameraIndex = 0, bool singleCamWrite = false) {
		return frontUiRenderer.SetTexture(pos, str, cameraIndex, CameraNum, singleCamWrite);
	}

	void ImageScale(const glm::vec2& scale) {
		uiRenderer.Scale(scale);
	}
	void FrontImageScale(const glm::vec2& scale) {
		frontUiRenderer.Scale(scale);
	}
	void ImageColor(const glm::vec4& color) {
		uiRenderer.Color(color);
	}
	void FrontImageColor(const glm::vec4& color) {
		frontUiRenderer.Color(color);
	}

	CameraData GetCameraInfo(int index) {
		return camera[index];
	}

	void toggleFullScreen();
	void EndGame();

	bool InitAudio(const char* acfPath, const char* acbPath, const char* awbPath,
		const char* dspBusName, size_t playerCount);
	void PlayAudio(int playerId, int cueId);
	void ResumeAudio(int playerId);
	void PauseAudio(int playerId);
	void StopAudio(int playerId);
	void StopAllSound();

	void SetCameraNum(int i);

	void AddEffect(glm::vec3 pos, glm::vec2 scale, glm::vec4 color, const char* imageName);

	int GetRandomInt(int maxInt, int minInt);
	float GetRandomFloat(float maxFloat, float minFloat, float acuuracy = 100.0f);

	double& Variable(const char* name) { return variable[name]; }

	Shader::ProgramPtr checkProgram(const char* program) {
		decltype(shaderMap)::const_iterator itr = shaderMap.end();
		if (program) {
			itr = shaderMap.find(program);
		}
		if (itr == shaderMap.end()) {
			itr = shaderMap.find("Tutorial");
			if (itr == shaderMap.end()) {
				return nullptr;
			}
		}
		return itr->second;
	}

private:
	GameEngine() = default;
	~GameEngine();
	GameEngine(const GameEngine&) = default;
	GameEngine& operator=(const GameEngine&) = default;
	void Update(double delta);
	void Render(float delta);

	void RenderCubeMap(int cameraNum) const;
	void RenderGround(int cameraNum) const;

private:
	bool isInitialized = false;
	UpdateFuncType updateFunc;

	bool fullScreen = false;
	bool running = true;

	int width = 0;
	int height = 0;
	GLuint vbo = 0;
	GLuint ibo = 0;
	GLuint vao = 0;

	GLuint vbo2 = 0;
	GLuint vao2 = 0;

	GLuint groundVbo = 0;
	GLuint groundVao = 0;

	Effect m_effect;

	int playerNum = 0;

	UniformBufferPtr uboLight;
	UniformBufferPtr uboPostEffect;

	TexturePtr cubemapTexture = 0;

	TexturePtr groundTexture0 = 0;
	TexturePtr groundTexture1 = 0;
	TexturePtr groundTexture2 = 0;
	TexturePtr groundTexture3 = 0;
	TexturePtr groundTexture4 = 0;

	std::unordered_map<std::string, Shader::ProgramPtr> shaderMap;

	OffscreenBufferPtr offscreen;
	
	std::unordered_map<std::string, TexturePtr> textureBuffer;
	Mesh::BufferPtr meshBuffer;
	Entity::BufferPtr entityBuffer;
	Font::Renderer fontRenderer;
	UITexture::Renderer uiRenderer;
	UITexture::Renderer frontUiRenderer;

	int CameraNum = 1;

	InterfaceBlock::LightData lightData;
	CameraData camera[4];

	std::unordered_map<std::string, double> variable;
};
