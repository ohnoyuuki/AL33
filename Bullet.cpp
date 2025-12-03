#include "Bullet.h"


Bullet::Bullet() {}

Bullet::~Bullet() {}

void Bullet::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, KamataEngine::Vector3& position) {
	

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// 因数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	worldTransform_.translation_ = position;

	// 速度を設定する
	velocity_ = {1.0f, 0.0f, 0.0f};

}

void Bullet::Update() {
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Bullet::Draw() { model_->Draw(worldTransform_, *camera_); 
}
