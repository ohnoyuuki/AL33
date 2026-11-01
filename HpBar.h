#pragma once
#include "KamataEngine.h"

class HpBar {
public:
	// デストラクタ
	~HpBar();

	// 初期化
	void Initialize(uint32_t textureHandle);

	// 更新
	void Update(uint32_t hp);

	// 描画
	void Draw();

private:
	// テクスチャー
	uint32_t textureHandle_;
	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;
	KamataEngine::Sprite* sprite2_ = nullptr;
};