#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include <vector>
#include"Skydome.h"
#include"MapChipField.h"
#include"CameraController.h"
#include "Enemy.h"
#include"DeathParticles.h"



class GameScene
{
public:

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Model* modelBlock_ = nullptr;
	// 3Dモデル
	KamataEngine::Model* modelSkydome_ = nullptr; 
	// モデルプレイヤー
	KamataEngine::Model* modelPlayer_ = nullptr;
	//モデル敵
	KamataEngine::Model* modelEnemy_ = nullptr;
	//デスパーティークル
	KamataEngine::Model* modelDeathparticles_ = nullptr; 


	// 自キャラ
	Player* player_ = nullptr;

	// キューブ
	Skydome* skydome_ = nullptr;

	// 敵
	std::list<Enemy*> enemies_;

	//全ての当たり判定を行う
	void CheckAllCollisions();

	// 表示ブロック
	void GenerateBlocks();

	MapChipField* mapChipField_;

	CameraController* cameraController_;

	DeathParticles* deathParticles_ = nullptr;


	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();
	// デストラクタ
	~GameScene();

	//デスフラグのgetter
	bool IsFinished() const { return finished_; }
	

private:
	



	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Camera camera_;

	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	bool isDebugCameraActive_ = false;

	// ゲームのフェーズ（型）
	enum class Phase {
		kPlay,  // ゲームプレイ
		kDeath, // デス演出
	};

	//ゲームの現在フェーズ（変数）
	Phase phase_;

	//フェーズの切り替え
	void ChangePhase();

	//終了フラグ
	bool finished_ = false;

};
