#include "GameScene.h"
#include "TextureManager.h"
#include "WinApp.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite_;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲（回転角用）
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	//乱数範囲（座標用）
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	textureHandle_ = TextureManager::Load("1-2/mario.jpg");

	//スプライトの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});

	// 3Dモデルの生成
	model_ = Model::Create();

	for (int i = 0; i < _countof(worldTransform_); i++) {
		// x,y,zのスケーリング設定
		worldTransform_[i].scale_ = {5.0f, 5.0f, 5.0f};

		// x,y,zの軸周りの回転角を設定
		worldTransform_[i].rotation_ = {0, 0, 0};

		// x,y,z軸周りの平行移動を設定
		if (i < 10) {
			worldTransform_[i].translation_ = { -50 + (float)i * worldTransform_[0].scale_.x * 2,
				-20, 0 };
		}
		else {
			worldTransform_[i].translation_ = { -50 + (float)(i - 10) * worldTransform_[0].scale_.x * 2,
				20, 0 };
		}

		//ワールドトランスフォームの初期化
		worldTransform_[i].Initialize();
	}

	////カメラの視点座標を設定
	viewProjection_.eye = {0, 0, -50};

	////カメラ注視点座標を設定
	viewProjection_.target = {0, 0, 0};

	////カメラ上方向ベクトルを設定（右上45度指定）
	// viewProjection_.up = { cosf(XM_PI / 4.0f), sinf(XM_PI / 4.0f), 0.0f };

	//カメラ垂直方向視野角を設定
	viewProjection_.fovAngleY = XMConvertToRadians(45.0f);

	//アスペクト比設定
	// viewProjection_.aspectRatio = 1.0f;

	////ニアクリップ距離を設定
	// viewProjection_.nearZ = 52.0f;

	////ファークリップ距離を設定
	// viewProjection_.farZ = 53.0f;

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {
	//スプライトの座標習得
	XMFLOAT2 position = sprite_->GetPosition();

	//座標を{2, 0}に移動
	position.x += 2.0f;
	position.y += 1.0f;

	//移動した座標をスプライトに反映
	sprite_->SetPosition(position);

	//デバックテキストの表示
	// debugText_->Print("Kaizokuou ni oreha naru.", 50, 50, 1.0f);

	//インクリメント
	value_++;
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>;

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	/// //3Dモデル描画
	for (int i = 0; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	// sprite_->Draw();

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}