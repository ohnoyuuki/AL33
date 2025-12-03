#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "MapChipField.h"
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

	// 速度
	Vector3 velocity_ = {};

private:

	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	Camera* camera_ = nullptr;
};
