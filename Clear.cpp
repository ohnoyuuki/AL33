#include "Math.h"
#include "Clear.h"
#include <numbers>

void ClearScene::Initialize() {
	// 3Dモデルの生成
	modelTitle_ = Model::CreateFromOBJ("titleFont", true);
	modelPlayer_ = Model::CreateFromOBJ("Enemy");

	// カメラの初期化
	camera_.Initialize();

	// ワールド変更の初期化
	worldTransformTitle_.Initialize();
	worldTransformTitle_.scale_ = {2, 2, 2};
	worldTransformTitle_.translation_ = {0, 8, 0};

	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.scale_ = {10, 10, 10};
	worldTransformPlayer_.translation_ = {0, -8, 0};
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float>;

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// 画像読み込み
	textureHandle_ = TextureManager::Load("Clear.png");

	// スプライトインスタンスの生成
	sprite_ = Sprite::Create(textureHandle_, {0, 0});
}

void ClearScene::Update() {

	switch (phase_) {

	case Phase::kMain:
		// タイトルシーンの終了条件
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			// フェードアウト開始
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;
	case Phase::kFadeIn:
		// フェード
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kFadeOut:
		// フェード
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
	}

	////タイトル終了
	// if (Input::GetInstance()->PushKey(DIK_SPACE)) {
	//	finished_ = true;
	// }

	//// フェード
	// fade_->Update();
	worldTransformTitle_.matWorld_ = MakeAffineMatrix(worldTransformTitle_.scale_, worldTransformTitle_.rotation_, worldTransformTitle_.translation_);
	worldTransformTitle_.TransferMatrix();

	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_, worldTransformPlayer_.translation_);
	worldTransformPlayer_.TransferMatrix();
}

void ClearScene::Draw() {

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommandList());

	sprite_->Draw();

	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
	// ３Dモデル描画前処理
	Model::PreDraw(dxCommon_->GetCommandList());

	// ここに３Dモデルインスタンスの描画処理を記述する
	/*modelTitle_->Draw(worldTransformTitle_, camera_);*/
	/*modelPlayer_->Draw(worldTransformPlayer_, camera_);*/
	// ３Dモデル描画後処理
	Model::PostDraw();

	fade_->Draw();
}

ClearScene::~ClearScene() {
	// モデル
	delete modelTitle_;
	delete modelPlayer_;

	delete sprite_;

	// フェード
	delete fade_;
}
