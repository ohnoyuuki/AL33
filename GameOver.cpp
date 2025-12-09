#include "GameOver.h"
#include "Math.h"
#include <numbers>

void GameOverScene::Initialize() {
	// --- 3Dモデルの生成（OBJファイルから読み込み） ---
	modelTitle_ = Model::CreateFromOBJ("titleFont", true); // タイトル文字のモデル
	//modelPlayer_ = Model::CreateFromOBJ("player");          // プレイヤー(敵)モデル

	// --- カメラ初期化 ---
	camera_.Initialize();

	// --- タイトル文字のワールド変換 ---
	worldTransformTitle_.Initialize();
	worldTransformTitle_.scale_ = {2, 2, 2};       // 拡大
	worldTransformTitle_.translation_ = {0, 8, 0}; // 上の方に配置

	// --- プレイヤーモデルのワールド変換 ---
	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.scale_ = {10, 10, 10};                   // 大きく表示
	worldTransformPlayer_.translation_ = {0, -8, 0};               // 下の方に配置
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float>; // Y軸で 180° 回転

	// --- フェードインの準備 ---
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f); // 1秒かけてフェードイン

	// --- 画像読み込み（2Dスプライト） ---
	textureHandle_ = TextureManager::Load("Over.png");

	// --- スプライト生成 ---
	sprite_ = Sprite::Create(textureHandle_, {0, 0}); // 画面左上に表示

	// サウンドデータの読み込み
	
	soundOverHandle_ = Audio::GetInstance()->LoadWave("ALOver.mp3");

	// --- 再生ハンドルは全部初期化しておく ---
	
	voiceOverHandle_ = -1;

	// タイトルBGMをループで流す
	voiceOverHandle_ = Audio::GetInstance()->PlayWave(soundOverHandle_, true);

}

void GameOverScene::Update() {
	switch (phase_) {

	case Phase::kMain:
		// --- メイン状態（キー待ち） ---
		// スペースキー押したらフェードアウト開始
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			// 音声停止
			Audio::GetInstance()->StopWave(voiceOverHandle_);
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

void GameOverScene::Draw() {
	// DirectXコマンドを取得
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();

	// --- スプライト描画開始 ---
	Sprite::PreDraw(dxCommon_->GetCommandList());

	// Clear.png を描画
	sprite_->Draw();

	// スプライト描画終了
	Sprite::PostDraw();

	// --- 深度バッファクリア ---
	dxCommon_->ClearDepthBuffer();

	// --- 3Dモデル描画準備 ---
	Model::PreDraw(dxCommon_->GetCommandList());

	// --- 3Dモデル描画終了 ---
	Model::PostDraw();

	// --- フェード描画 ---
	fade_->Draw();
}

GameOverScene::~GameOverScene() {
	// --- モデルの破棄 ---
	delete modelTitle_;
	delete modelPlayer_;

	// --- スプライト破棄 ---
	delete sprite_;

	// --- フェード破棄 ---
	delete fade_;
}
