#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
//#include "MapChipField.h"
#include"Enemy.h"
#include <algorithm>
#include <numbers>


using namespace KamataEngine;

class Enemy;
class Bullet {

public:
	Bullet();
	~Bullet();

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, KamataEngine::Vector3& position);
	void Update();
	void Draw();

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// 速度
	Vector3 velocity_ = {};

	bool isActive_ = false;

	//リセット
	void Reset(const Vector3& pos) {
		worldTransform_.translation_ = pos;
		isActive_ = true;
	}

	//AABBを取得
	AABB GetAABB();

	//衝突応答
	void OnCollision(const Enemy* enemy);
	
	//発射方向を設定する関数
	void Fire(const Vector3& pos, bool isRight);
	void FireHorizontal(const Vector3& pos, bool isRight);

private:

	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	Camera* camera_ = nullptr;

	// デスフラグ
	bool isDead_ = false;

	// 経過時間
	float respawnTimer_ = 0.0f;

	// 復活までの時間（秒）
	static inline const float kRespawnTime = 3.0f;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 弾が出現してからの時間
	float lifeTimer_ = 0.0f;

	// 弾の生存時間
	static constexpr float kLifeTime = 1.0f;
};
