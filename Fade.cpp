#include "Fade.h"
#include <algorithm>

using namespace KamataEngine;

//----------------------------------------------
// 初期化
//----------------------------------------------
void Fade::Initialize() {

	// --- 白1x1テクスチャを読み込み（後から黒色で描画するための素材） ---
	textureHandle_ = TextureManager::Load("white1x1.png");

	// --- スプライト生成（画面全体に表示するための板ポリ） ---
	sprite_ = Sprite::Create(textureHandle_, {0, 0});

	// --- スプライトのサイズを画面サイズに合わせる（1280×720） ---
	sprite_->SetSize(Vector2(1280, 720));

	// --- 初期色は黒 + α1（完全不透明） ---
	sprite_->SetColor(Vector4(0, 0, 0, 1));
}

//----------------------------------------------
// 更新処理（フェードの進行）
//----------------------------------------------
void Fade::Update() {

	// フェード状態によって処理を変える
	switch (status_) {

	case Status::None:
		// 何もしていない状態 → 処理なし
		break;

	case Status::FadeIn:
		// --- フェードイン（黒 → 透明） ---

		// 1フレームの経過時間を加算
		counter_ += 1.0f / 60.0f;

		// 指定時間に達したら終了位置で固定
		if (counter_ >= duration_) {
			counter_ = duration_;
		}

		// 経過時間に応じて 黒→透明 へ変化させる
		// (counter_ / duration_) が 0 → 1 に変化する
		sprite_->SetColor(Vector4(0, 0, 0, 1.0f - std::clamp(counter_ / duration_, 0.0f, 1.0f)));

		break;

	case Status::FadeOut:
		// --- フェードアウト（透明 → 黒） ---

		// 経過時間を加算
		counter_ += 1.0f / 60.0f;

		// 打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}

		// 経過時間に応じて 透明→黒 に変化
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));

		break;
	}
}

//----------------------------------------------
// 描画処理
//----------------------------------------------
void Fade::Draw() {

	// None の時はフェードを描かない
	if (status_ == Status::None) {
		return;
	}

	// DirectX のコマンド取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// --- スプライト描画開始 ---
	Sprite::PreDraw(dxCommon->GetCommandList());

	// --- 黒い板を描画（透明度は Update() で設定した値） ---
	sprite_->Draw();

	// --- スプライト描画終了 ---
	Sprite::PostDraw();
}

//----------------------------------------------
// フェード開始
//----------------------------------------------
void Fade::Start(Status status, float duration) {

	status_ = status;     // フェードイン or フェードアウト
	duration_ = duration; // 何秒かけてフェードするか
	counter_ = 0.0f;      // 経過時間をリセット
}

//----------------------------------------------
// フェード強制終了
//----------------------------------------------
void Fade::Stop() { status_ = Status::None; }

//----------------------------------------------
// フェード終了判定
//----------------------------------------------
bool Fade::IsFinished() const {

	switch (status_) {

	case Status::FadeIn:
	case Status::FadeOut:
		// 経過時間が duration 以上になったら終了
		return (counter_ >= duration_);
	}

	// None状態は常に終了扱い
	return true;
}
