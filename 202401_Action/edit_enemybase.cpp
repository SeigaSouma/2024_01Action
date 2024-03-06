//=============================================================================
// 
//  敵の拠点エディター処理 [edit_enemybase.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "edit_enemybase.h"
#include "enemybase.h"
#include "objectX.h"
#include "XLoad.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "input.h"
#include "elevation.h"
#include "game.h"
#include "enemymanager.h"
#include "enemy.h"
#include "player.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define MOVE		(10.0f)
#define MOVE_SLOW	(1.0f)
#define MARKOBJ	"data\\MODEL\\koko.x"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CEditEnemyBase::CEditEnemyBase()
{
	// ゼロクリア
	m_pos = mylib_const::DEFAULT_VECTOR3;	// 位置
	m_rot = mylib_const::DEFAULT_VECTOR3;			// 位置
	m_nEnemyType = 0;			// 追従の種類
	m_nIdxMapPoint = 0;			// マップポイントのインデックス番号
	m_fPointRatio = 0.0f;		// 移動割合
	m_fMoveValue = 0.0f;		// 移動量
	memset(&m_pEnemy[0], 0, sizeof(m_pEnemy));	// 敵へのポインタ
	m_apObjX = nullptr;			// オブジェクトX
	m_nStage = 0;				// ステージ
	m_nRush = 0;				// ラッシュ用かどうか
}

//==========================================================================
// デストラクタ
//==========================================================================
CEditEnemyBase::~CEditEnemyBase()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEditEnemyBase *CEditEnemyBase::Create()
{
	// 生成用のオブジェクト
	CEditEnemyBase *pObjBase = nullptr;

	if (pObjBase == nullptr)
	{// nullptrだったら

		// メモリの確保
		pObjBase = DEBUG_NEW CEditEnemyBase;

		if (pObjBase != nullptr)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pObjBase->Init();
			if (FAILED(hr))
			{// 失敗していたら
				return nullptr;
			}
		}

		return pObjBase;
	}

	return nullptr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEditEnemyBase::Init()
{
	m_pos = CManager::GetInstance()->GetCamera()->GetPositionR();
	m_apObjX = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
	m_apObjX->SetType(CObject::TYPE_OBJECT3D);
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEditEnemyBase::Uninit()
{
	
}

//==========================================================================
// 終了処理
//==========================================================================
void CEditEnemyBase::Release()
{
	if (m_apObjX != nullptr)
	{
		m_apObjX->Uninit();
		m_apObjX = nullptr;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CEditEnemyBase::Update()
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 操作
	Control(m_pos);
	ChangeType();

	// 目印
	m_apObjX->SetPosition(m_pos);

	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_PATTEN_ENEMY; nCntEnemy++)
	{
		if (m_pEnemy[nCntEnemy] != nullptr)
		{// nullptrじゃなかったら
			m_pEnemy[nCntEnemy]->SetPosition(m_pos);
		}
	}

	// 掴み処理
	if (pInputKeyboard->GetPress(DIK_SPACE) == true)
	{// SPACEで掴む
		Grab();
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{// ENTERで配置

		// 敵の拠点の取得
		CEnemyBase *pEnemyBase = CGame::GetInstance()->GetEnemyBase();
		if (pEnemyBase == nullptr)
		{
			return;
		}
		// 生成
		pEnemyBase->CreatePos(m_nStage, m_nEnemyType, m_pos, m_nRush);
	}

	if (pInputKeyboard->GetTrigger(DIK_DELETE) == true)
	{// deleteで削除
		Delete();
	}

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// セーブ

		// 敵の拠点の取得
		CEnemyBase *pEnemyBase = CGame::GetInstance()->GetEnemyBase();
		if (pEnemyBase == nullptr)
		{
			return;
		}
		pEnemyBase->Save();
	}

	// デバッグ情報
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[ 敵の拠点エディター情報 ]------------------\n"
		"<配置>         [ENTER]\n"
		"<ファイル保存> [F9] 【data/BIN/enemybase.bin】\n"
		"<移動>         高速：[↑,↓,←,→]　低速：[W,A,S,D]\n"
		"<上昇,下降>    [I,K]\n"
		"<種類変更>     [1, 2][%d]\n"
		"<ステージ変更> [3, 4][%d]\n"
		"<影の使用状況> [5][%d]\n"
		"<掴み移動>     [SPACE]\n"
		"<削除>         [DELETE]\n"
		"<位置>         [X：%f Y：%f Z：%f]\n"
		"\n", m_nEnemyType, m_nStage, m_nRush, m_pos.x, m_pos.y, m_pos.z);
}

//==========================================================================
// 操作処理
//==========================================================================
void CEditEnemyBase::Control(MyLib::Vector3 &pos)
{
	// カメラの情報取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// カメラの向き取得
	MyLib::Vector3 Camerarot = pCamera->GetRotation();

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (pInputKeyboard->GetPress(DIK_LEFT) == true)
	{// ←キーが押された,左移動

		if (pInputKeyboard->GetPress(DIK_UP) == true)
		{// A+W,左上移動

			pos.x += sinf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			pos.z += cosf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// A+S,左下移動

			pos.x += sinf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			pos.z += cosf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
		}
		else
		{// A,左移動

			pos.x += sinf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			pos.z += cosf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
	{// Dキーが押された,右移動

		if (pInputKeyboard->GetPress(DIK_UP) == true)
		{// D+W,右上移動

			pos.x += sinf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// D+S,右下移動

			pos.x += sinf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
		}
		else
		{// D,右移動

			pos.x += sinf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_UP) == true)
	{// Wが押された、奥移動

		pos.x += sinf(Camerarot.y) * MOVE;
		pos.z += cosf(Camerarot.y) * MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
	{// Sが押された、手前移動

		pos.x += sinf(D3DX_PI + Camerarot.y) * MOVE;
		pos.z += cosf(D3DX_PI + Camerarot.y) * MOVE;
	}

	if (pInputKeyboard->GetPress(DIK_A) == true)
	{// ←キーが押された,左移動

		if (pInputKeyboard->GetPress(DIK_W) == true)
		{// A+W,左上移動

			pos.x += sinf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{// A+S,左下移動

			pos.x += sinf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
		}
		else
		{// A,左移動

			pos.x += sinf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_D) == true)
	{// Dキーが押された,右移動

		if (pInputKeyboard->GetPress(DIK_W) == true)
		{// D+W,右上移動

			pos.x += sinf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{// D+S,右下移動

			pos.x += sinf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
		}
		else
		{// D,右移動

			pos.x += sinf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_W) == true)
	{// Wが押された、奥移動

		pos.x += sinf(Camerarot.y) * MOVE_SLOW;
		pos.z += cosf(Camerarot.y) * MOVE_SLOW;
	}
	else if (pInputKeyboard->GetPress(DIK_S) == true)
	{// Sが押された、手前移動

		pos.x += sinf(D3DX_PI + Camerarot.y) * MOVE_SLOW;
		pos.z += cosf(D3DX_PI + Camerarot.y) * MOVE_SLOW;
	}

	if (pInputKeyboard->GetPress(DIK_I) == true)
	{// Iキーが押された,上昇

		pos.y += MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_K) == true)
	{// Kキーが押された,下降

		pos.y -= MOVE;
	}

}


//==========================================================================
// 種類変更
//==========================================================================
void CEditEnemyBase::ChangeType()
{
	int nNumAll = CGame::GetInstance()->GetEnemyManager()->GetPatternNum();

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 配置するオブジェクト変更
	bool bChange = false;
	if (pInputKeyboard->GetTrigger(DIK_1) == true)
	{// 1が押された

		// 色の種類更新
		m_nEnemyType = (m_nEnemyType + (nNumAll - 1)) % nNumAll;
		bChange = true;
	}
	else if (pInputKeyboard->GetTrigger(DIK_2) == true)
	{// 2が押された

		// 色の種類更新
		m_nEnemyType = (m_nEnemyType + 1) % nNumAll;
		bChange = true;
	}

	if (pInputKeyboard->GetTrigger(DIK_3) == true)
	{// 3が押された

		// 色の種類更新
		m_nStage--;
	}
	else if (pInputKeyboard->GetTrigger(DIK_4) == true)
	{// 4が押された

		// 色の種類更新
		m_nStage++;
	}

	if (pInputKeyboard->GetTrigger(DIK_5) == true)
	{// 5が押された

		// 影の使用状況切り替え
		m_nRush = m_nRush ? 0 : 1;
	}

	if (bChange)
	{
		for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_PATTEN_ENEMY; nCntEnemy++)
		{
			if (m_pEnemy[nCntEnemy] != nullptr)
			{// nullptrにする
				m_pEnemy[nCntEnemy]->Kill();
				m_pEnemy[nCntEnemy]->Uninit();
				m_pEnemy[nCntEnemy] = nullptr;
			}
		}

		CEnemyManager *pEnemyManager = CGame::GetInstance()->GetEnemyManager();

		// 敵配置
		CEnemy **ppEnemy = pEnemyManager->SetEnemy(m_pos, m_rot, m_nEnemyType);

		for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_PATTEN_ENEMY; nCntEnemy++)
		{
			if (ppEnemy[nCntEnemy] != nullptr)
			{
				m_pEnemy[nCntEnemy] = ppEnemy[nCntEnemy];
			}
		}
	}

}

//==========================================================================
// モデル掴む
//==========================================================================
void CEditEnemyBase::Grab()
{

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 敵の拠点の取得
	CEnemyBase *pEnemyBase = CGame::GetInstance()->GetEnemyBase();
	if (pEnemyBase == nullptr)
	{
		return;
	}

	bool bAll = false;
	if (pInputKeyboard->GetPress(DIK_RCONTROL) == true)
	{// 右コントロールで全選択
		bAll = true;
	}

	for (int i = 0; i < pEnemyBase->GetNumBase(m_nStage); i++)
	{
		MyLib::Vector3 TargetPoint = pEnemyBase->GetSpawnPoint(m_nStage, i);
		if (bAll == true || UtilFunc::Collision::SphereRange(m_pos, TargetPoint, 50.0f, 50.0f).ishit)
		{// 球に当たってたら

			// 操作
			Control(TargetPoint);

			// 位置設定
			pEnemyBase->SetSpawnPoint(m_nStage, i, TargetPoint);
		}
	}
}

//==========================================================================
// モデル掴む
//==========================================================================
void CEditEnemyBase::Delete()
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 敵の拠点の取得
	CEnemyBase *pEnemyBase = CGame::GetInstance()->GetEnemyBase();
	if (pEnemyBase == nullptr)
	{
		return;
	}

	for (int i = 0; i < pEnemyBase->GetNumAll(); i++)
	{
		MyLib::Vector3 TargetPoint = pEnemyBase->GetSpawnPoint(m_nStage, i);
		if (UtilFunc::Collision::SphereRange(m_pos, TargetPoint, 50.0f, 50.0f).ishit)
		{// 球に当たってたら
			pEnemyBase->DeletePos(m_nStage, i);
		}
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CEditEnemyBase::Draw()
{

}
