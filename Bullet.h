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

private:

	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	Camera* camera_ = nullptr;

	// デスフラグ
	bool isDead_ = false;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
};
