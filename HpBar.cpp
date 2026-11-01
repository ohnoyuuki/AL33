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
	sprite_ = Sprite::Create(textureHandle_, {40, 20});
	sprite2_ = Sprite::Create(textureHandle_, {40, 20});
}

// 更新
void HpBar::Update(uint32_t hp) {
	// 最大HP
	const float maxHP = 3.0f;

	// HPバーの最大サイズ
	const float maxWidth = 200.0f;

	// 背景の設定
	sprite_->SetColor({1.0f, 0.0f, 0.0f, 0.5f});
	sprite_->SetSize({maxWidth, 30});

	// HPに応じた横幅を計算
	float hpWidth = maxWidth * (static_cast<float>(hp) / maxHP);

	// HPバーの設定
	sprite2_->SetColor({0.0f, 1.0f, 0.0f, 0.7f});
	sprite2_->SetSize({hpWidth, 30});
}

// 描画
void HpBar::Draw() {
	sprite_->Draw();
	sprite2_->Draw();
}