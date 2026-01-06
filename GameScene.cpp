#include "GameScene.h"
#include "MyMath.h"

using namespace KamataEngine;
// 初期化/////////////////////////////////////////////////////////////
void GameScene::Initialize() {

	// モデル読み込み（汎用モデル）
	model_ = Model::Create();

	// ブロック
	modelBlock_ = Model::CreateFromOBJ("yuki");

	// 天球（スカイドーム）
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// プレイヤー
	modelPlayer_ = Model::CreateFromOBJ("roboto");

	// 弾
	modelBullet_ = Model::CreateFromOBJ("tama");

	// 敵
	modelEnemy_ = Model::CreateFromOBJ("boss");

	// デスパーティクル
	modelDeathparticles_ = Model::CreateFromOBJ("deathParticle");

	worldTransform_.Initialize();

	// 自キャラの生成
	player_ = new Player();

	// 弾の生成
	bullet_ = new Bullet();

	// スカイドーム生成
	skydome_ = new Skydome();
	// 初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// デバッグカメラ
	debugCamera_ = new DebugCamera(1280, 720);

	// マップチップ読込
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// ブロック生成（マップを形にする）
	GenerateBlocks();

	// プレイヤーの初期位置（マップチップから取得）座標をマップ地プ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(3, 18);
	player_->Initialize(modelPlayer_, &camera_, playerPosition);

	// 弾の初期位置
	Vector3 bulletPosition = mapChipField_->GetMapChipPositionByIndex(4, 18);
	bullet_->Initialize(modelBullet_, &camera_, bulletPosition);

	// カメラ
	camera_.Initialize();

	cameraController_ = new CameraController();

	cameraController_->Initialize();

	cameraController_->SetTarget(player_);

	cameraController_->Reset();

	// カメラ移動範囲（マップ外に出ないため）
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	// マップチップデータのセット
	// 自キャラの生成と初期化（当たり判定用）
	player_->SetMapChipField(mapChipField_);

	// 敵生成（等間隔に5体）
	for (int32_t i = 0; i < 5; i++) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(20 + i * 17, 18);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	// ゲームプレイフェーズから開始（フェードインから）
	phase_ = Phase::kFadeIn;

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// ゴール配置
	Vector3 goalPosition = mapChipField_->GetMapChipPositionByIndex(77, 18);
	worldTransformGoal_.Initialize();
	worldTransformGoal_.translation_ = goalPosition;
	worldTransformGoal_.scale_ = {0.5f, 0.5f, 0.5f};

	// 画像読み込み
	textureHandle_ = TextureManager::Load("Goal.png");
	modelGoal_ = Model::Create();

	// サウンドデータの読み込み

	soundGameHandle_ = Audio::GetInstance()->LoadWave("ALGame.mp3");

	//soundTamaHandle_ = Audio::GetInstance()->LoadWave("p_death.mp3");

	// --- 再生ハンドルは全部初期化しておく ---

	voiceGameHandle_ = -1;

	// タイトルBGMをループで流す
	voiceGameHandle_ = Audio::GetInstance()->PlayWave(soundGameHandle_, true);
}

// 更新//////////////////////////////////////////////////////////////////////////////////////////////////////
void GameScene::Update() {

	// フェード
	fade_->Update();
	ChangePhase(); // フェーズ変更処理

	switch (phase_) {

	case Phase::kPlay:

		// ゲームプレイフェーズの処理////////////////////////////////////
		// 天球の更新
		skydome_->Update();

		// 自キャラの更新
		player_->Update();

		// 弾の更新
		bullet_->Update();

		// スペースキーで弾を発射（位置をリセット）
		// if (!bullet_->isActive_) {
		if (Input::GetInstance()->TriggerKey(DIK_J)) {
			Vector3 pos = player_->GetWorldPosition();
			bullet_->Reset(pos);
		}
		//}

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		// カメラコントローラーの更新
		cameraController_->Update();

		// デバッグカメラ操作（F1キーなどで切替）
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

		// プレイヤーが死んだら弾消える
		if (player_->IsDead()) {
			
			bullet_->isActive_ = false;
			//Audio::GetInstance()->PlayWave(soundTamaHandle_);
			// 音声停止
			Audio::GetInstance()->StopWave(soundGameHandle_);
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
		// 弾の更新
		bullet_->Update();

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

	// ゲームクリア
	if (player_->GetWorldPosition().x >= 77) {
		finished_ = true;
		gameClear_ = true;
		// 音声停止
		Audio::GetInstance()->StopWave(soundGameHandle_);
	}
}

// 描画/////////////////////////////////////////////////////////////////////////////////
void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

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

	// プレイヤー（死んでない時描画）
	if (!player_->IsDead()) {
		player_->Draw();
	}

	// 弾
	bullet_->Draw();

	// 敵
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

	delete bullet_;

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

	Audio::GetInstance()->StopWave(voiceGameHandle_);
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
		// 音声停止

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

	#pragma region 弾と敵の当たり判定
	if (!bullet_->isActive_) {
		return;
	}

	AABB bulletAABB = bullet_->GetAABB();

	for (Enemy* enemy : enemies_) {

		if (!enemy->IsAlive()) {
			continue; // ★ 死んでる敵は無視
		}

		AABB enemyAABB = enemy->GetAABB();

		if (IsCollision(bulletAABB, enemyAABB)) {
			bullet_->OnCollision(enemy);
			enemy->OnCollision(bullet_);
			break;
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