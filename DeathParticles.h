#pragma once
#include "KamataEngine.h"
#include<array>
#include"MyMath.h"
#include<numbers>
#include<algorithm>


using namespace KamataEngine;
using namespace MathUtility;


class DeathParticles {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine ::Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();

	


private:

	// モデル
	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	std::array<WorldTransform, kNumParticles> worldTransforms_;

	//存続時間（消滅までの時間）＜秒＞
	static inline const float kDuration = 1.0f;

	//移動の速さ
	static inline const float kSpeed = 0.2f;

	//分割した１個分の角度
	static inline const float kAngleUnit = 2.0f * std::numbers::pi_v<float> / kNumParticles;

	//終了フラグ
	bool isFinished_ = false;

	//経過時間カウント
	float counter_ = 0.0f;

	//色変更オブジェクト
	ObjectColor objectColor_;
	//色の数値
	Vector4 color_;



};
