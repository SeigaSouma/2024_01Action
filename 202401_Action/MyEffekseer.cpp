//=============================================================================
// 
//  エフェクシア処理 [MyEffekseer.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "MyEffekseer.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "pause.h"
#include <Effekseer.h>

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
std::string CMyEffekseer::m_EffectName[CMyEffekseer::EFKLABEL_MAX] =	// エフェクトのファイル名
{
	"data/Effekseer/Laser01.efkefc",			// サンプルのレーザー
	"data/Effekseer/CounterParticle_01.efkefc",	// カウンターの線
	"data/Effekseer/counter2.efkefc",			// カウンターの線2
	"data/Effekseer/CounterParticle_02.efkefc",	// カウンターのキラキラ
	"data/Effekseer/CounterBreak.efkefc",		// カウンターブレイク
	"data/Effekseer/BackgroundFire.efkefc",		// 背景の炎
	"data/Effekseer/HitParticle_Red_01.efkefc",	// ヒットマーク[赤]
	"data/Effekseer/strongATK.efkefc",			// 強攻撃のサイン
	"data/Effekseer/SonicBoom.efkefc",			// ボスのローリング
	"data/Effekseer/stonebase_light.efkefc",	// 石板の光
	"data/Effekseer/stonebase_begin.efkefc",	// 石板起動
	"data/Effekseer/transfer.efkefc",			// 石板起動
	"data/Effekseer/chargeatk_01.efkefc",		// チャージ完了
	"data/Effekseer/powerloop.efkefc",			// チャージ完了
	"data/Effekseer/titlelight.efkefc",			// タイトルの光
	"data/Effekseer/enemyatk_sand.efkefc",		// 敵攻撃の煙
	"data/Effekseer/normalATK_left.efkefc",		// 通常攻撃左振り
	"data/Effekseer/normalATK_right.efkefc",	// 通常攻撃右振り
	"data/Effekseer/normalATK.efkefc",			// 通常攻撃
	"data/Effekseer/chargeatk.efkefc",			// チャージ攻撃
	"data/Effekseer/respawn_start.efkefc",		// 復活開始
	"data/Effekseer/respawn_wind.efkefc",		// 復活開始
};
CMyEffekseer* CMyEffekseer::m_pMyEffekseer = nullptr;	// 自身のポインタ

//==========================================================================
// コンストラクタ
//==========================================================================
CMyEffekseer::CMyEffekseer()
{
	// 変数のクリア
	time = 0;
	efkHandle = 0;
	m_Handle.clear();		// エフェクトのハンドル
}

//==========================================================================
// デストラクタ
//==========================================================================
CMyEffekseer::~CMyEffekseer()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CMyEffekseer* CMyEffekseer::Create()
{
	if (m_pMyEffekseer == nullptr)
	{// まだ生成していなかったら

		// インスタンス生成
		m_pMyEffekseer = DEBUG_NEW CMyEffekseer;

		// 初期化処理
		m_pMyEffekseer->Init();
	}
	else
	{
		// インスタンス取得
		m_pMyEffekseer->GetInstance();
	}

	return m_pMyEffekseer;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CMyEffekseer::Init()
{
	time = 0;
	efkHandle = 0;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// Effekseerのオブジェクトはスマートポインタで管理される。変数がなくなると自動的に削除される。

	// エフェクトのマネージャーの作成
	efkManager = ::Effekseer::Manager::Create(8000);

	// Effekseerのモジュールをセットアップする
	SetupEffekseerModules(efkManager);
	auto efkRenderer = GetEffekseerRenderer();

	// 座標系を設定する。アプリケーションと一致させる必要がある。
	efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	// 視点位置を確定
	viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	// 投影行列を設定
	projectionMatrix.PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 500.0f);

	// カメラ行列を設定
	cameraMatrix.LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// エフェクトの設定
//==========================================================================
Effekseer::Handle CMyEffekseer::SetEffect(EFKLABEL label, MyLib::Vector3 pos, MyLib::Vector3 rot, MyLib::Vector3 move, float scale, bool bAutoDeath)
{
	MyEffekseerInfo loacalInfo = {};

	// 読み込み
	loacalInfo.efcRef = LoadEffect(m_EffectName[label]);

	// エフェクトの再生
	loacalInfo.handle = efkManager->Play(m_LoadEffect, 0.0f, 0.0f, 0.0f);

	// 引数情報設定
	loacalInfo.pos = pos;
	loacalInfo.rot = rot;
	loacalInfo.move = move;
	loacalInfo.scale = scale;
	loacalInfo.bAutoDeath = bAutoDeath;
	efkManager->SetLocation(loacalInfo.handle, pos.x, pos.y, pos.z);
	efkManager->SetRotation(loacalInfo.handle, rot.x, rot.y, rot.z);
	efkManager->SetScale(loacalInfo.handle, scale, scale, scale);

	// 要素追加
	m_EffectObj.push_back(loacalInfo);
	m_Handle.push_back(loacalInfo.handle);		// エフェクトのハンドル

	return loacalInfo.handle;
}

//==========================================================================
// エフェクトの設定
//==========================================================================
Effekseer::Handle CMyEffekseer::SetEffect(Effekseer::Handle** pHandle, EFKLABEL label, MyLib::Vector3 pos, MyLib::Vector3 rot, MyLib::Vector3 move, float scale, bool bAutoDeath)
{
	MyEffekseerInfo loacalInfo = {};

	// 読み込み
	loacalInfo.efcRef = LoadEffect(m_EffectName[label]);

	// エフェクトの再生
	loacalInfo.handle = efkManager->Play(m_LoadEffect, 0.0f, 0.0f, 0.0f);

	// 引数情報設定
	loacalInfo.pos = pos;
	loacalInfo.rot = rot;
	loacalInfo.move = move;
	loacalInfo.scale = scale;
	loacalInfo.bAutoDeath = bAutoDeath;
	efkManager->SetLocation(loacalInfo.handle, pos.x, pos.y, pos.z);
	efkManager->SetRotation(loacalInfo.handle, rot.x, rot.y, rot.z);
	efkManager->SetScale(loacalInfo.handle, scale, scale, scale);

	// 要素追加
	m_EffectObj.push_back(loacalInfo);
	m_Handle.push_back(loacalInfo.handle);		// エフェクトのハンドル

	int idx = m_Handle.size() - 1;
	*pHandle = &m_Handle[idx];	// ポインタに最後の情報を渡す

	return loacalInfo.handle;
}

//==========================================================================
// 読み込み処理
//==========================================================================
Effekseer::EffectRef CMyEffekseer::LoadEffect(std::string efkpath)
{
	// char16_tに変換
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
	std::u16string string16t = converter.from_bytes(efkpath);

	// エフェクトの読込
	auto effect = Effekseer::Effect::Create(efkManager, string16t.c_str());
	m_LoadEffect = effect;

	onLostDevice = [effect]() -> void
	{
		// 読み込んだエフェクトのリソースは全て破棄する。
		if (effect != nullptr)
		{
			effect->UnloadResources();
		}
	};

	onResetDevice = [effect]() -> void
	{
		// エフェクトのリソースを再読み込みする。
		if (effect != nullptr)
		{
			effect->ReloadResources();
		}
	};

	return effect;
}

//==========================================================================
// 終了処理
//==========================================================================
void CMyEffekseer::Uninit()
{
	efkManager->StopAllEffects();

	delete m_pMyEffekseer;
	m_pMyEffekseer = nullptr;
}

//==========================================================================
// 停止
//==========================================================================
void CMyEffekseer::Stop(Effekseer::Handle handle)
{
	for (int i = 0; i < static_cast<int>(m_EffectObj.size()); i++)
	{
		Effekseer::Handle loacalhandle = m_EffectObj[i].handle;
		if (loacalhandle == handle)
		{
			// 削除
			m_EffectObj.erase(m_EffectObj.begin() + i);
			m_Handle.erase(m_Handle.begin() + i);
		}
	}

	// 停止
	efkManager->StopEffect(handle);
}

//==========================================================================
// 全て停止
//==========================================================================
void CMyEffekseer::StopAll()
{
	// 全て停止
	efkManager->StopAllEffects();

	for (int i = 0; i < static_cast<int>(m_EffectObj.size()); i++)
	{
		Effekseer::Handle loacalhandle = m_EffectObj[i].handle;
		efkManager->StopEffect(loacalhandle);
	}

	int size = static_cast<int>(m_EffectObj.size());
	for (int i = 0; i < size; i++)
	{
		m_EffectObj[i].bAutoDeath = true;
	}
	m_EffectObj.clear();
	m_Handle.clear();
}

//==========================================================================
// 更新処理
//==========================================================================
void CMyEffekseer::Update()
{
	if (!CManager::GetInstance()->GetPause()->IsPause())
	{

		// キーボード情報取得
		CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
		float size = 10.0f;

		for (int i = 0; i < static_cast<int>(m_EffectObj.size()); i++)
		{
			Effekseer::Handle loacalhandle = m_EffectObj[i].handle;

			if (!efkManager->Exists(loacalhandle))
			{// 再生終了

				efkManager->StopEffect(loacalhandle);

				// 削除
				if (m_EffectObj[i].bAutoDeath)
				{
					m_EffectObj.erase(m_EffectObj.begin() + i);
					m_Handle.erase(m_Handle.begin() + i);
				}
				else
				{

					// 新たにエフェクトを再生し直す。座標はエフェクトを表示したい場所を設定する
					// (位置や回転、拡大縮小も設定しなおす必要がある)
					m_Handle[i] = efkManager->Play(m_EffectObj[i].efcRef, 0.0f, 0.0f, 0.0f);
					m_EffectObj[i].handle = m_Handle[i];
					efkManager->SetLocation(m_EffectObj[i].handle, m_EffectObj[i].pos.x, m_EffectObj[i].pos.y, m_EffectObj[i].pos.z);
					efkManager->SetRotation(m_EffectObj[i].handle, m_EffectObj[i].rot.x, m_EffectObj[i].rot.y, m_EffectObj[i].rot.z);
					efkManager->SetScale(m_EffectObj[i].handle, m_EffectObj[i].scale, m_EffectObj[i].scale, m_EffectObj[i].scale);
				}
			}
			else
			{// 再生中

				if (!m_EffectObj[i].move.IsNearlyZero(0.01f))
				{
					// エフェクトの移動
					efkManager->AddLocation(
						loacalhandle,
						Effekseer::Vector3D(m_EffectObj[i].move.x, m_EffectObj[i].move.y, m_EffectObj[i].move.z));
				}


				Effekseer::Vector3D scale;

				efkManager->GetMatrix(m_EffectObj[i].handle).GetScale(scale);

				efkManager->SetScale(m_EffectObj[i].handle, m_EffectObj[i].scale, m_EffectObj[i].scale, m_EffectObj[i].scale);
			}
		}

		// レイヤーパラメータの設定
		Effekseer::Manager::LayerParameter layerParameter;
		::Effekseer::Matrix44 invEfkCameraMatrix;
		::Effekseer::Matrix44::Inverse(invEfkCameraMatrix, cameraMatrix);
		layerParameter.ViewerPosition = ::Effekseer::Vector3D(invEfkCameraMatrix.Values[3][0], invEfkCameraMatrix.Values[3][1], invEfkCameraMatrix.Values[3][2]);
		efkManager->SetLayerParameter(0, layerParameter);

		// マネージャーの更新
		Effekseer::Manager::UpdateParameter updateParameter;
		efkManager->Update(updateParameter);
	}

	// 描画処理
	Draw();

	if (!CManager::GetInstance()->GetPause()->IsPause())
	{
		time++;
	}
}

//==========================================================================
// トリガー送信
//==========================================================================
void CMyEffekseer::SetTrigger(Effekseer::Handle handle, int idx)
{
	if (!efkManager->Exists(handle))
	{// 再生終了
		return;
	}

	efkManager->SendTrigger(handle, idx);
}

//==========================================================================
// 位置更新
//==========================================================================
void CMyEffekseer::SetPosition(Effekseer::Handle handle, MyLib::Vector3 pos)
{
	if (!efkManager->Exists(handle))
	{// 再生終了
		return;
	}

	// インデックス検索
	std::vector<Effekseer::Handle>::iterator itr = std::find(m_Handle.begin(), m_Handle.end(), handle);
	int idx = std::distance(m_Handle.begin(), itr);

	// 位置情報設定
	m_EffectObj[idx].pos = pos;

	efkManager->SetLocation(handle, Effekseer::Vector3D(pos.x, pos.y, pos.z));
}

//==========================================================================
// 向き更新
//==========================================================================
void CMyEffekseer::SetRotation(Effekseer::Handle handle, MyLib::Vector3 rot)
{
	if (!efkManager->Exists(handle))
	{// 再生終了
		return;
	}

	efkManager->SetRotation(handle, rot.x, rot.y, rot.z);
}

//==========================================================================
// マトリックス設定
//==========================================================================
void CMyEffekseer::SetMatrix(Effekseer::Handle handle, D3DXMATRIX mtx)
{
	if (!efkManager->Exists(handle))
	{// 再生終了
		return;
	}

	// 軌跡のマトリックス取得
	Effekseer::Matrix43 efcmtx;
	efcmtx = CMyEffekseer::GetInstance()->GetMatrix(handle);

	// 4x3行列に向きを設定
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			efcmtx.Value[i][j] = mtx.m[i][j];
		}
	}

	// エフェクトのインスタンスに変換行列を設定
	efkManager->SetMatrix(handle, efcmtx);

	// インデックス検索
	std::vector<Effekseer::Handle>::iterator itr = std::find(m_Handle.begin(), m_Handle.end(), handle);
	int idx = std::distance(m_Handle.begin(), itr);

	// スケール情報設定
	float scale = m_EffectObj[idx].scale;
	efkManager->SetScale(handle, scale, scale, scale);
}

//==========================================================================
// マトリックス取得
//==========================================================================
Effekseer::Matrix43 CMyEffekseer::GetMatrix(Effekseer::Handle handle)
{
	return efkManager->GetMatrix(handle);
}

//==========================================================================
// スケール設定
//==========================================================================
void CMyEffekseer::SetScale(Effekseer::Handle handle, float scale)
{
	efkManager->SetScale(handle, scale, scale, scale);
}

//==========================================================================
// マトリックス設定
//==========================================================================
void CMyEffekseer::SetTransform(Effekseer::Handle handle, MyLib::Vector3 pos, MyLib::Vector3 rot)
{
	Effekseer::Matrix43 a;
	a.Translation(pos.x, pos.y, pos.z);
	a.RotationZXY(rot.x, rot.y, rot.z);

	Effekseer::Matrix43 Weapon = efkManager->GetMatrix(handle);
	//Weapon.Indentity();

	Weapon.Multiple(Weapon, Weapon, a);

	efkManager->SetMatrix(handle, Weapon);
}

//==========================================================================
// 終了フラグ取得
//==========================================================================
bool CMyEffekseer::IsDeath(Effekseer::Handle handle)
{
	return !efkManager->Exists(handle);
}

//==========================================================================
// 描画処理
//==========================================================================
void CMyEffekseer::Draw()
{
	// 時間を更新する
	efkRenderer->SetTime(time / 60.0f);

	// 投影行列を設定
	efkRenderer->SetProjectionMatrix(projectionMatrix);

	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	D3DXMATRIX appProjectionMatrix = pCamera->GetMtxProjection();

	// EffekseerのCameraMatrixは、カメラの姿勢行列の逆行列を指す。カメラ行列がカメラの姿勢行列である場合は、逆行列化しておく。
	auto invAppCameraMatrix = pCamera->GetMtxView();

	// 自分の環境とEffekseerの、投影(射影)行列とカメラ行列を同期
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			projectionMatrix.Values[i][j] = appProjectionMatrix.m[i][j];
			cameraMatrix.Values[i][j] = invAppCameraMatrix.m[i][j];
		}
	}

	// カメラ行列を設定
	efkRenderer->SetCameraMatrix(cameraMatrix);

	// エフェクトの描画開始処理を行う。
	efkRenderer->BeginRendering();

	// エフェクトの描画を行う。
	Effekseer::Manager::DrawParameter drawParameter;
	drawParameter.ZNear = 0.0f;
	drawParameter.ZFar = 1.0f;
	drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();
	efkManager->Draw(drawParameter);

	// エフェクトの描画終了処理
	efkRenderer->EndRendering();
}

//==========================================================================
// モジュールのセットアップ
//==========================================================================
void CMyEffekseer::SetupEffekseerModules(::Effekseer::ManagerRef efkManager)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 描画デバイスの作成
	auto graphicsDevice = ::EffekseerRendererDX9::CreateGraphicsDevice(pDevice);

	// エフェクトのレンダラーの作成
	efkRenderer = ::EffekseerRendererDX9::Renderer::Create(graphicsDevice, 8000);

	// 描画モジュールの設定
	efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
	efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
	efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
	efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
	efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

	// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
	efkManager->SetModelLoader(efkRenderer->CreateModelLoader());
	efkManager->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
	efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
}
