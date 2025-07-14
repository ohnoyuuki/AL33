#include "Enemy.h"
#include "MapChipField.h"
#include "MyMath.h"
#include <algorithm>
#include <numbers>


//初期化
void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, KamataEngine::Vector3& position) {


}

//更新
void Enemy::Update(){




	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
}
// 描画
void Enemy::Draw(){

	model_->Draw(worldTransform_, *camera_); }
