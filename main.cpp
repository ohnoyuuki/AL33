#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
#include"Clear.h"

using namespace KamataEngine;

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
ClearScene* clearScene = nullptr;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
	kClear,
};

// 現在シーン（型）
Scene scene = Scene::kUnknown;

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// シーン変更

			if (gameScene->IsClear()) {
				scene = Scene::kClear;
				clearScene = new ClearScene;
				clearScene->Initialize();

			} else {
				scene = Scene::kTitle;
				titleScene = new TitleScene;
				titleScene->Initialize();
			}
		

			
			delete gameScene;
			gameScene = nullptr;
		}
		break;
	case Scene::kClear:
		if (clearScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			delete clearScene;
			clearScene = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	}
}

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
	}
}
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
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// ゲームシーンのインスタンス生成

	// エンジンの初期化
	KamataEngine::Initialize(L"LE2D_08_オオノ_ユウキ_ENEMYBREAK");
	// 最初のシーンの初期化
	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();

	//// ゲームシーンの初期化
	// gameScene->Initialize();

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {

			break;
		}

		// titleScene->Update();
		//   シーン切り替え
		ChangeScene();
		// シーン更新
		UpdateScene();

		//// ゲームシーンの更新
		// gameScene->Update();

		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		// titleScene->Draw();

		// シーンの描画
		DrawScene(); //

		//// ゲームシーンの描画
		// gameScene->Draw();

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
