#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
#include"Clear.h"
#include"GameOver.h"

using namespace KamataEngine;

// 各シーンのインスタンスを管理するポインタ
// nullptrは、まだインスタンスが作られていない状態を表す
GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
ClearScene* clearScene = nullptr;
GameOverScene* overScene = nullptr;

// シーンの種類を管理する列挙型
enum class Scene {
	kUnknown = 0,
	kTitle,// タイトル
	kGame,// ゲーム本編
	kClear,// クリア
	kOver,// ゲームオーバー
};

// 現在どのシーンを表示しているかを管理する変数
Scene scene = Scene::kUnknown;

// シーン切り替え処理
// 現在のシーンが終了したかを確認し、次のシーンへ切り替える
void ChangeScene() {

	switch (scene) {
		//タイトルシーン
	case Scene::kTitle:
		// タイトルシーンが終了状態なら
		if (titleScene->IsFinished()) {
			// ゲームシーンへ切り替え
			scene = Scene::kGame;

			// 使用しなくなったタイトルシーンを削除する
			delete titleScene;
			titleScene = nullptr;

			// 新しいゲームシーンのインスタンスを生成する
			gameScene = new GameScene;

			// ゲームシーンの初期化を行う
			gameScene->Initialize();
		}
		break;

		//ゲームシーン
	case Scene::kGame:

		
		// ゲームシーンが終了したか確認する
		if (gameScene->IsFinished()) {
			// シーン変更

			 // ゲームをクリアしたか確認する
			if (gameScene->IsClear()) {

				// クリアシーンに切り替える
				scene = Scene::kClear;

				// クリアシーンのインスタンスを生成する
				clearScene = new ClearScene;

				// クリアシーンの初期化を行う
				clearScene->Initialize();

			} else {
				// クリアできなかった場合はゲームオーバーシーンに切り替える
				scene = Scene::kOver;

				// ゲームオーバーシーンのインスタンスを生成する
				overScene = new GameOverScene;

				// ゲームオーバーシーンの初期化を行う
				overScene->Initialize();
			}
		

			// 使用しなくなったゲームシーンを削除する
			delete gameScene;
			gameScene = nullptr;
		}
		break;
		//クリアシーン
	case Scene::kClear:

		// クリアシーンが終了したか確認する
		if (clearScene->IsFinished()) {

			// タイトルシーンに戻る
			scene = Scene::kTitle;

			// 使用しなくなったクリアシーンを削除する
			delete clearScene;
			clearScene = nullptr;

			// 新しいタイトルシーンのインスタンスを生成する
			titleScene = new TitleScene;

			// タイトルシーンの初期化を行う
			titleScene->Initialize();
		}
		break;
		//オーバーシーン
	case Scene::kOver:

		// ゲームオーバーシーンが終了したか確認する
		if (overScene->IsFinished()) {
			
			// タイトルシーンに戻る
			scene = Scene::kTitle;
			// 使用しなくなったゲームオーバーシーンを削除する
			delete overScene;
			overScene = nullptr;

			// 新しいタイトルシーンのインスタンスを生成する
			titleScene = new TitleScene;

			// タイトルシーンの初期化を行う
			titleScene->Initialize();
		}
	}
}

// シーンの更新処理
// 現在表示しているシーンのUpdate()だけを呼び出す
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
// 現在のシーンに対応したDraw()だけを呼び出す
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
	KamataEngine::Initialize(L"LE3D_06_オオノ_ユウキ_Game");
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
