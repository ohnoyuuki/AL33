#include "GameOver.h"

void GameOverScene::Initialize() {}

void GameOverScene::Update() {
	switch (phase_) {

	case Phase::kMain:
		// --- メイン状態（キー待ち） ---
		// スペースキー押したらフェードアウト開始
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f); // 1秒フェードアウト
		}
		break;

	case Phase::kFadeIn:
		// --- フェードイン中 ---
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain; // フェードが終わったらメインへ
		}
		break;

	case Phase::kFadeOut:
		// --- フェードアウト中 ---
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true; // シーン終了
		}
		break;
	}

	// --- タイトルモデルのワールド行列更新 ---
	worldTransformTitle_.matWorld_ = MakeAffineMatrix(worldTransformTitle_.scale_, worldTransformTitle_.rotation_, worldTransformTitle_.translation_);
	worldTransformTitle_.TransferMatrix();

	// --- プレイヤーモデルのワールド行列更新 ---
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_, worldTransformPlayer_.translation_);
	worldTransformPlayer_.TransferMatrix();
}

void GameOverScene::Draw() {}

GameOverScene::~GameOverScene() {}
