#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
//#include "MapChipField.h"
#include"Enemy.h"
#include <algorithm>
#include <numbers>


using namespace KamataEngine;

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

	//void OnCollision(Enemy* enemy) { isActive_ = false; }


private:

	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	Camera* camera_ = nullptr;
};
