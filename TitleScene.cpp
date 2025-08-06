#include "TitleScene.h"
#include <numbers>
#include "Math.h"

void TitleScene::Initialize() {
	//3Dモデルの生成
	modelTitle_ = Model::CreateFromOBJ("titleFont", true);
	modelPlayer_ = Model::CreateFromOBJ("player");

	//カメラの初期化
	camera_.Initialize();

	//ワールド変更の初期化
	worldTransformTitle_.Initialize();
	worldTransformPlayer_.Initialize();


}

void TitleScene::Update() {

	//タイトル終了
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}

}

void TitleScene::Draw() {

	//DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();

	//３Dモデル描画前処理
	Model::PreDraw(dxCommon_->GetCommandList());

	//ここに３Dモデルインスタンスの描画処理を記述する
	modelTitle_->Draw(worldTransformTitle_, camera_);
	modelPlayer_->Draw(worldTransformPlayer_, camera_);
	//３Dモデル描画後処理
	Model::PostDraw();

}
