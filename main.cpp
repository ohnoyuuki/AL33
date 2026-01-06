#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
#include"Clear.h"
#include"GameOver.h"

using namespace KamataEngine;

// 各シーンのインスタンス（ポインタ）
GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
ClearScene* clearScene = nullptr;
GameOverScene* overScene = nullptr;

// シーンの種類
enum class Scene {
	kUnknown = 0,
	kTitle,// タイトル
	kGame,// ゲーム本編
	kClear,// クリア
	kOver,// ゲームオーバー
};

// 現在のシーンを示す変数
Scene scene = Scene::kUnknown;

// シーン切り替え処理
void ChangeScene() {
	switch (scene) {
		//タイトルシーン
	case Scene::kTitle:
		// タイトルシーンが終了状態なら
		if (titleScene->IsFinished()) {
			// ゲームシーンへ切り替え
			scene = Scene::kGame;

			// 古いシーン削除
			delete titleScene;
			titleScene = nullptr;

			// 新しいゲームシーン作成＆初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;

		//ゲームシーン
	case Scene::kGame:

		// ゲームシーンが終了したか
		if (gameScene->IsFinished()) {
			// シーン変更

			 // クリアしたか？
			if (gameScene->IsClear()) {

				// クリアシーンへ
				scene = Scene::kClear;
				clearScene = new ClearScene;
				clearScene->Initialize();

			} else {
				// 失敗 → オーバーシーンへ
				scene = Scene::kOver;
				overScene = new GameOverScene;
				overScene->Initialize();
			}
		

			 // 古いゲームシーン削除
			delete gameScene;
			gameScene = nullptr;
		}
		break;
		//クリアシーン
	case Scene::kClear:
		if (clearScene->IsFinished()) {

			// タイトルへ戻る
			scene = Scene::kTitle;
			// 古いクリアシーン削除
			delete clearScene;
			clearScene = nullptr;

			// 新しいタイトルシーン作成
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
		//オーバーシーン
	case Scene::kOver:
		if (overScene->IsFinished()) {
			
			// タイトルへ戻る
			scene = Scene::kTitle;
			// 古いオーバシーン削除
			delete overScene;
			overScene = nullptr;

			// 新しいタイトルシーン作成
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
	}
}

// シーンの更新処理
void UpdateScene() {

	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kClear:
		clearScene->Update();
		break;
	case Scene::kOver:
		overScene->Update();
		break;
	}
}
// シーンの描画処理
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kClear:
		clearScene->Draw();
		break;
	case Scene::kOver:
		overScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// ゲームシーンのインスタンス生成

	// エンジンの初期化
	KamataEngine::Initialize(L"LE2D_05_オオノ_ユウキ_WinterGame");
	// 最初のシーンの初期化
	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();


	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {

			break;
		}

		//   シーン切り替え
		ChangeScene();
		// シーン更新
		UpdateScene();


		// 描画開始
		dxCommon->PreDraw();


		// シーンの描画
		DrawScene(); 


		// 描画終了
		dxCommon->PostDraw();
	}
	// ゲームシーンの解放
	delete titleScene;
	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}
