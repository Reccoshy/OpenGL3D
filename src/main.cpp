/*
@file	main.cpp
*/
#include "GameEngine.h"
#include "Game/Scene/RaceScene/RaceScene.h"
#include "Game/Scene/TitleScene/TitleScene.h"
#include "../res/Sound/ElemantalDriver_acf.h"
#include "../res/Sound/Sound.h"
#include <random>
#include <time.h>
#include <glm/gtc/matrix_transform.hpp>

int main(){

	srand(time(NULL));

	GameEngine& game = GameEngine::Instance();
	if (!game.Init(1920, 1080, "OpenGL Tutorial", false)) {
		system("pause");
		return 1;
	}

	if (!game.InitAudio("res/Sound/ElemantalDriver.acf", "res/Sound/Sound.acb", "res/Sound/Sound.awb",
		CRI_ELEMANTALDRIVER_ACF_DSPSETTING_DSPBUSSETTING_0, 8)) {
		system("pause");
		return 1;
	}

	game.LoadTextureFromFile("res/Model/Toroid.bmp");

	game.LoadTextureFromFile("res/Texture/Ground/PlanePlaneField/goal.bmp");
	game.LoadTextureFromFile("res/Texture/Ground/PlanePlaneField/dirt.bmp");
	game.LoadTextureFromFile("res/Texture/Ground/PlanePlaneField/grass.bmp");
	game.LoadTextureFromFile("res/Texture/Ground/PlanePlaneField/flower.bmp");		
	game.LoadTextureFromFile("res/Texture/Ground/PlanePlaneField/testMap.bmp");

	game.LoadTextureFromFile("res/Model/Toroid.Normal.bmp");
	game.LoadTextureFromFile("res/Model/Dummy.Normal.bmp");
	game.LoadTextureFromFile("res/Model/Player.bmp");
	game.LoadTextureFromFile("res/Model/SpaceSphere.bmp");
	game.LoadTextureFromFile("res/Model/Ground.bmp");
	game.LoadTextureFromFile("res/Model/dainamaito.bmp");
	game.LoadTextureFromFile("res/Model/Tree1.bmp");


	game.LoadTextureFromFile("res/Model/Test.bmp");

	std::vector<const char*> DayCubeMap{
		"res/Texture/CubeMap/Day/right.bmp",
		"res/Texture/CubeMap/Day/left.bmp",
		"res/Texture/CubeMap/Day/top.bmp",
		"res/Texture/CubeMap/Day/bottom.bmp",
		"res/Texture/CubeMap/Day/front.bmp",
		"res/Texture/CubeMap/Day/back.bmp"
	};
	game.LoadCubeMap(DayCubeMap, "CubeMap");

	std::vector<const char*> NightCubeMap{
		"res/Texture/CubeMap/Night/right.bmp",
		"res/Texture/CubeMap/Night/left.bmp",
		"res/Texture/CubeMap/Night/top.bmp",
		"res/Texture/CubeMap/Night/bottom.bmp",
		"res/Texture/CubeMap/Night/front.bmp",
		"res/Texture/CubeMap/Night/back.bmp"
	};
	game.LoadCubeMap(NightCubeMap, "Night");

	game.LoadFontFromFile("res/Font.fnt");

	game.LoadTextureFromFile("res/Texture/MiniMap/PlanePlaneField/Map1UiC.dds");
	game.LoadTextureFromFile("res/Texture/MiniMap/PlanePlaneField/mapPlayerDotUi.dds");
	game.LoadTextureFromFile("res/Texture/menuSelector.dds");
	game.LoadTextureFromFile("res/Texture/images.dds");
	game.LoadTextureFromFile("res/Texture/ArrorL.dds");
	game.LoadTextureFromFile("res/Texture/ArrorR.dds");
	game.LoadTextureFromFile("res/Texture/SpeedMeter.dds");

	game.LoadTextureFromFile("res/Texture/itemGetSparkle.dds");

	game.LoadTextureFromFile("res/Texture/respawn1.dds");
	game.LoadTextureFromFile("res/Texture/respawn2.dds");
	game.LoadTextureFromFile("res/Texture/respawn3.dds");
	game.LoadTextureFromFile("res/Texture/respawn4.dds");
	game.LoadTextureFromFile("res/Texture/respawn5.dds");
	game.LoadTextureFromFile("res/Texture/respawn6.dds");
	game.LoadTextureFromFile("res/Texture/respawn7.dds");
	game.LoadTextureFromFile("res/Texture/respawn8.dds");
	game.LoadTextureFromFile("res/Texture/respawn9.dds");

	game.LoadMeshFromFile("res/Model/CartoonLowPolyTrees.fbx");
	game.LoadMeshFromFile("res/Model/Toroid.fbx");
	game.LoadMeshFromFile("res/Model/Player.fbx");
	game.LoadMeshFromFile("res/Model/Blast.fbx");

	game.LoadMeshFromFile("res/Model/StraightRoad.fbx");
	game.LoadMeshFromFile("res/Model/Cube.fbx");
	game.LoadMeshFromFile("res/Model/dainamaito.fbx");
	game.LoadMeshFromFile("res/Model/Shadow.fbx");

	game.LoadMeshFromFile("res/Model/Explode.fbx");

	game.UpdateFunc(TitleScene());
	
	//game.UpdateFunc(RaceScene(
	//	"res/Data/data.txt",
	//	"res/Data/StageData/PlanePlaneField/TestStage1.txt",
	//	"res/Data/StageData/PlanePlaneField/TestCheckPoint1.txt",
	//	"res/Data/StageData/PlanePlaneField/TestItems1.txt",
	//	"res/Data/StageData/PlanePlaneField/TestStageTextures1.txt",
	//	3, 2));
	//
	/*
	game.UpdateFunc(RaceScene("res/Data/StageData/StarLightTown/Stage.txt",
		"res/Data/StageData/StarLightTown/CheckPoint.txt",
		"res/Data/StageData/StarLightTown/Items.txt",
		"res/Data/StageData/StarLightTown/StageTextures.txt",
		0, 2));
	*/
	game.Run();

	return 0;
}