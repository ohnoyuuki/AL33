#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "MyMath.h"

using namespace KamataEngine;

class GameOverScene {
public:
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

	void Initialize();
	void Update();
	void Draw();
	~GameOverScene();

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
	  
	    uint32_t soundOverHandle_ = 0;

	    // 音声再生ハンドル
	   
	    int voiceOverHandle_ = 0;


};