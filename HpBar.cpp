#include "HpBar.h"
using namespace KamataEngine;

// デストラクタ
HpBar::~HpBar() {
	delete sprite_;
	delete sprite2_;
}

// 初期化
void HpBar::Initialize(uint32_t textureHandle) {
	textureHandle_ = textureHandle;
	// スプライト
	sprite_ = Sprite::Create(textureHandle_, {100, 20});
	sprite2_ = Sprite::Create(textureHandle_, {100, 20});
}

// 更新
void HpBar::Update(uint32_t hp) {
	sprite_->SetColor({1.0f, 0.0f, 0.0f, 0.5f});
	sprite_->SetSize({200, 30});
	sprite2_->SetColor({0.0f, 1.0f, 0.0f, 0.7f});
	sprite2_->SetSize({(float)hp, 30});
}

// 描画
void HpBar::Draw() {
	sprite_->Draw();
	sprite2_->Draw();
}