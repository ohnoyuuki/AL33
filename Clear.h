#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "MyMath.h"

using namespace KamataEngine;

// クリアシーンを表すクラス
class ClearScene {
public:
	// シーンの進行段階（フェーズ）
	enum class Phase {
		kFadeIn,  // シーン開始時のフェードイン
		kMain,    // メイン表示中
		kFadeOut, // 終了前のフェードアウト
	};

	// 初期化処理
	void Initialize();

	// フレームごとの更新処理
	void Update();

	// 描画処理
	void Draw();

	// デストラクタ（メモリ解放などを行う）
	~ClearScene();

	// シーンが終了したかどうか（外部から取得）
	bool IsFinished() const { return finished_; }

private:
	// タイトルモデルが動くまでの時間（秒）
	static inline const float kTimeTitleMove = 2.0f;

	// カメラのビュー・プロジェクション管理
	Camera camera_;

	// タイトル文字のワールド変換（位置・回転・拡縮）
	WorldTransform worldTransformTitle_;

	// プレイヤーモデルのワールド変換
	WorldTransform worldTransformPlayer_;

	// プレイヤーモデル（3Dモデル）
	Model* modelPlayer_ = nullptr;

	// タイトルモデル（3Dモデル）
	Model* modelTitle_ = nullptr;

	// 時間計測用のカウンター
	float counter_ = 0.0f;

	// シーン終了フラグ（true なら次のシーンへ）
	bool finished_ = false;

	// フェード制御クラス
	Fade* fade_ = nullptr;

	// 現在のシーンフェーズ
	Phase phase_ = Phase::kFadeIn;

	// 画像の読み込みハンドル（テクスチャID）
	uint32_t textureHandle_ = 0;

	// スプライト（2D画像表示用）
	KamataEngine::Sprite* sprite_ = nullptr;

	// 音声ファイル
	
	uint32_t soundClearHandle_ = 0;
	
	// 音声再生ハンドル
	
	int voiceClearHandle_ = 0;

	// 効果音ハンドル
	uint32_t soundBotanHandle_ = 0;

};
