#include "GameScene.h"
#include "MyMath.h"

using namespace KamataEngine;
// 初期化/////////////////////////////////////////////////////////////
void GameScene::Initialize() {

	model_ = KamataEngine::Model::Create();

	modelBlock_ = Model::CreateFromOBJ("block");

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	modelPlayer_ = Model::CreateFromOBJ("player");

	modelEnemy_ = Model::CreateFromOBJ("enemy");

	modelDeathparticles_ = Model::CreateFromOBJ("deathParticle");

	worldTransform_.Initialize();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化

	// 生成
	skydome_ = new Skydome();
	// 初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	debugCamera_ = new DebugCamera(1280, 720);

	mapChipField_ = new MapChipField;

	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	GenerateBlocks();

	// 座標をマップ地プ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(3, 18);
	player_->Initialize(modelPlayer_, &camera_, playerPosition);

	camera_.Initialize();

	cameraController_ = new CameraController();

	cameraController_->Initialize();

	cameraController_->SetTarget(player_);

	cameraController_->Reset();

	// カメラ移動範囲
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	// マップチップデータのセット
	// 自キャラの生成と初期化
	player_->SetMapChipField(mapChipField_);

	// Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(15, 18);
	// enemy_->Initialize(modelEnemy_, &camera_, enemyPosition);

	for (int32_t i = 0; i < 5; i++) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(20 + i * 17, 18);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	// ゲームプレイフェーズから開始
	phase_ = Phase::kFadeIn;

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	Vector3 goalPosition = mapChipField_->GetMapChipPositionByIndex(77, 18);
	worldTransformGoal_.Initialize();
	worldTransformGoal_.translation_ = goalPosition;
	worldTransformGoal_.scale_ = {0.5f, 0.5f, 0.5f};

	// 画像読み込み
	textureHandle_ = TextureManager::Load("Goal.png");

	modelGoal_ = Model::Create();
}

// 更新//////////////////////////////////////////////////////////////////////////////////////////////////////
void GameScene::Update() {

	// フェード
	fade_->Update();
	ChangePhase();

	switch (phase_) {
	case Phase::kPlay:
		// ゲームプレイフェーズの処理////////////////////////////////////
		// 天球の更新
		skydome_->Update();
		// 自キャラの更新
		player_->Update();
		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		// カメラコントローラーの更新
		cameraController_->Update();
		// カメラの更新
		debugCamera_->Update();
#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_0)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif // _DEBUG
		if (isDebugCameraActive_) {
			// デバックカメラの更新
			debugCamera_->Update();
			// デバックカメラのビュー行列
			camera_.matView = debugCamera_->GetCamera().matView;
			// デバックカメラのプロジェクション行列
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			camera_.matView = cameraController_->GetViewProjection().matView;
			camera_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		}
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				/*	worldTransformBlock->matWorld_ = アフィン変換行列　*/

				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		// 全ての当たり判定
		CheckAllCollisions();

		break;
	case Phase::kDeath:
		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;

			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		// デス演出フェーズの処理////////////////////////////////////
		//  //天球の更新
		skydome_->Update();
		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// デスパーティクルの更新
		if (deathParticles_) {
			deathParticles_->Update();
		}

		// カメラの更新////////////////////
		debugCamera_->Update();
#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_0)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif // _DEBUG
		if (isDebugCameraActive_) {
			// デバックカメラの更新
			debugCamera_->Update();
			// デバックカメラのビュー行列
			camera_.matView = debugCamera_->GetCamera().matView;
			// デバックカメラのプロジェクション行列
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			camera_.matView = cameraController_->GetViewProjection().matView;
			camera_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		}
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				/*	worldTransformBlock->matWorld_ = アフィン変換行列　*/

				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		break;
	case Phase::kFadeIn:
		// ゲームプレイフェーズの処理////////////////////////////////////
		// 天球の更新
		skydome_->Update();
		// 自キャラの更新
		player_->Update();
		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		// カメラコントローラーの更新
		cameraController_->Update();
		// カメラの更新
		debugCamera_->Update();
#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_0)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif // _DEBUG
		if (isDebugCameraActive_) {
			// デバックカメラの更新
			debugCamera_->Update();
			// デバックカメラのビュー行列
			camera_.matView = debugCamera_->GetCamera().matView;
			// デバックカメラのプロジェクション行列
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			camera_.matView = cameraController_->GetViewProjection().matView;
			camera_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		}
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				/*	worldTransformBlock->matWorld_ = アフィン変換行列　*/

				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		// 全ての当たり判定
		CheckAllCollisions();

		break;
	}

	worldTransformGoal_.matWorld_ = MakeAffineMatrix(worldTransformGoal_.scale_, worldTransformGoal_.rotation_, worldTransformGoal_.translation_);
	worldTransformGoal_.TransferMatrix();

	if (player_->GetWorldPosition().x >= 77) {
		finished_ = true;
		gameClear_ = true;
	}
}

// 描画/////////////////////////////////////////////////////////////////////////////////
void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// player_->Draw();
	//  ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	skydome_->Draw();

	if (!player_->IsDead()) {
		player_->Draw();
	}

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// モデルパーティクル
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	/*modelEnemy_->Draw(worldTransformGoal_, camera_);*/

	modelGoal_->Draw(worldTransformGoal_, camera_, textureHandle_);
	Model::PostDraw();

	// フェード
	fade_->Draw();
}
// デストラクタ////////////////////////////////////////////////////////////////////////////////
GameScene::~GameScene() {
	delete model_;
	// 自キャラの解放
	delete player_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	delete debugCamera_;

	delete skydome_;

	delete modelSkydome_;

	delete mapChipField_;

	delete cameraController_;

	// 解放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	delete deathParticles_;

	// フェード
	delete fade_;

	delete modelGoal_;
}

void GameScene::ChangePhase() { ///////////////////////////////////////////////////////////
	switch (phase_) {
	case Phase::kPlay:
		// ゲームプレイフェーズの処理
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			// 自キャラの座標にデスパーティクルを発生、初期化
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathparticles_, &camera_, deathParticlesPosition);
		}

		break;

	case Phase::kDeath:
		// デス演出フェーズの処理

		break;

	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			// ゲームプレイ
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kFadeOut:
		// シーン終了
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}

void GameScene::CheckAllCollisions() {
#pragma region 自キャラと敵キャラの当たり判定
	// 判定対象１と２の座標
	AABB aabb1, aabb2;

	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵弾全ての当たり判定
	for (Enemy* enemy : enemies_) {
		// 敵弾の座標
		aabb2 = enemy->GetAABB();
		// AABB同士の交差判定
		if (IsCollision(aabb1, aabb2)) {
			// 自キャラの衝突時間関数を呼び出す
			player_->OnCollision(enemy);
			// 敵弾の衝突時コールバックを呼び出す
			enemy->OnCollision(player_);
		}
	}

#pragma endregion
}

void GameScene::GenerateBlocks() {
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	// 要素数を変更する
	// 列数を設定（縦方向のブロック数）
	worldTransformBlocks_.resize(numBlockVirtical);

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 一列の要素数を設定（横方向のブロック数）
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (UINT32 j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}