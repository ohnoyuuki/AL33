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
	velocity_ = {-0.5f, 0.0f, 0.0f};

}

void Bullet::Update() {

	if (!isActive_) {
		return; // 非表示なら処理なし
	}
		
	
	//=== 移動 ===//
	worldTransform_.translation_.x -= velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;




	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Bullet::Draw() {
	if (isActive_) {
		model_->Draw(worldTransform_, *camera_);
	};
}

 Vector3 Bullet::GetWorldPosition() {//ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
 }

 AABB Bullet::GetAABB() {
	 Vector3 worldPos = GetWorldPosition();

	 AABB aabb;
	 aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	 aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	 return aabb;
 }

 void Bullet::OnCollision(const Enemy* enemy) {
	 (void)enemy;
	 // 当たったら消える　
	 isActive_ = false; // 弾を消す

 }
