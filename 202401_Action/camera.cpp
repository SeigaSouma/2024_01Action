//=============================================================================
// 
// カメラ処理 [camera.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "calculation.h"
#include "player.h"
#include "elevation.h"
#include "enemymanager.h"
#include "title.h"
#include "instantfade.h"
#include "light.h"
#include "3D_effect.h"
#include "calculation.h"
#include "limitarea.h"
#include "pause.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define MOVE			(2.5f)				// 移動量
#define MAX_LENGTH		(50000.0f)			// 最大距離
#define MIN_LENGTH		(10.0f)				// 最少距離
#define START_CAMERALEN	(800.0f)			// 元の距離
#define ROT_MOVE_MOUSE	(0.01f)				// 回転移動量
#define ROT_MOVE_STICK_Y	(0.00040f)			// 回転移動量
#define ROT_MOVE_STICK_Z	(0.00020f)			// 回転移動量
#define ROT_MOVE		(0.025f)			// 回転移動量
#define MIN_STICKROT			(-D3DX_PI * 0.25f)	// カメラ固定用
#define MIN_ROT			(-D3DX_PI * 0.49f)	// カメラ固定用
#define MAX_ROT			(D3DX_PI * 0.49f)	// カメラ固定用
#define BACKFOLLOW_TIME	(20)				// 背面補正までの時間
#define TITLE_POSR_DEST	(MyLib::Vector3(321.91f, 160.74f, -296.28f))
#define TITLE_LEN_DEST	(365.0f)
#define TITLECAMERAROT_NONE		(MyLib::Vector3(0.0f, -0.76f, 0.24f))
#define TITLECAMERAROT_ENEMY	(MyLib::Vector3(0.0f, -0.79f, -0.12f))
#define TITLESTATE_CHANGE	(60 * 14)
#define TITLESTATE_CHASE	(60 * 20)
#define RESULT_LEN	(500.0f)
#define RANKING_LEN_DEST	(1000.0f)
#define RANKING_POS_V	(MyLib::Vector3(70.0f, 362.0f, -160.0f))
//#define RESULT_LEN	(1000.0f)
#define RANKINGROT_NONE		(MyLib::Vector3(0.0f, 0.15f, 0.06f))
#define DECIDECAMERAROT_NONE		(MyLib::Vector3(0.0f, 0.0f, 0.0f))
#define DECIDECAMERAPOS_NONE		(MyLib::Vector3(0.0f, 230.0f, -50.0f))
#define DECIDE_LEN	(500.0f)


namespace
{
	const MyLib::Vector3 DEFAULT_GAMEROT = MyLib::Vector3(0.0f, 0.0f, -0.20f);	// ゲームのデフォルト向き
	const float MULTIPLY_POSV_CORRECTION = 2.1f;	// (ゲーム時)視点の補正係数倍率
	const float MULTIPLY_POSR_CORRECTION = 2.1f;	// (ゲーム時)注視点の補正係数倍率
	const float DISATNCE_POSR_PLAYER = 200.0f;		// (ゲーム時)プレイヤーとの注視点距離
	const float MIN_ROCKONDISTANCE = 1.0f;
	const float ROTDISTANCE_ROCKON = D3DX_PI * 0.095f;	// ロックオン向きのズレ
	const MyLib::Vector3 ROTDISTANCE_COUNTER = MyLib::Vector3(0.0f, D3DX_PI * 0.5f, -D3DX_PI * 0.05f);	// 反撃時の向きズレ
	const float LENGTH_COUNTER = 400.0f;					// カウンター時のカメラ長さ
	const MyLib::Vector3 ROTATION_PRAYER = MyLib::Vector3(0.0f, -0.89f, 0.06f);	// 祈り時の向き
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CCamera::ROCKON_STATE_FUNC CCamera::m_RockOnStateFunc[] =	// カウンター状態
{
	&CCamera::RockOnStateNormal,	// 通常
	&CCamera::RockOnStateCounter,	// カウンター
};

//==========================================================================
// コンストラクタ
//==========================================================================
CCamera::CCamera()
{
	m_viewport.X = 0;							// 描画する画面の左上X座標
	m_viewport.Y = 0;							// 描画する画面の左上Y座標
	m_viewport.Width = 0;						// 描画する画面の幅
	m_viewport.Height = 0;						// 描画する画面の高さ
	m_viewport.Width = 0;						// 描画する画面の幅
	m_viewport.Height = 0;						// 描画する画面の高さ
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 0.0f;
	m_posR = mylib_const::DEFAULT_VECTOR3;		// 注視点(見たい場所)
	m_posV = mylib_const::DEFAULT_VECTOR3;		// 視点(カメラの位置)
	m_posVDest = mylib_const::DEFAULT_VECTOR3;	// 目標の視点
	m_posRDest = mylib_const::DEFAULT_VECTOR3;	// 目標の注視点
	m_vecU = MyLib::Vector3(0.0f, 1.0f, 0.0f);		// 上方向ベクトル
	m_move = mylib_const::DEFAULT_VECTOR3;		// 移動量
	m_rot = mylib_const::DEFAULT_VECTOR3;		// 向き
	m_rotDest = 0.0f;							// 目標の向き
	m_Moverot = 0.0f;							// 向きの移動量
	m_rotVDest = mylib_const::DEFAULT_VECTOR3;	// 目標の視点の向き
	m_TargetPos = mylib_const::DEFAULT_VECTOR3;	// 追従目標の位置
	m_TargetRot = mylib_const::DEFAULT_VECTOR3;	// 追従目標の位置
	m_fDistance = 0.0f;							// 距離
	m_fDestDistance = 0.0f;						// 目標の距離
	m_fOriginDistance = 0.0f;					// 元の距離
	m_nShakeLength = 0.0f;						// 揺れの長さ
	m_nShakeLengthY = 0.0f;						// Yの揺れの長さ
	m_fMoveShake = 0.0f;						// 揺れの移動量
	m_fMoveShakeY = 0.0f;						// Yの揺れの移動量
	m_fHeightMaxDest = 0.0f;					// カメラの最大高さの目標
	m_fHeightMax = 0.0f;						// カメラの最大高さ
	m_fDiffHeight = 0.0f;						// 高さの差分
	m_fDiffHeightSave = 0.0f;					// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;					// 目標の高さの差分
	m_bFollow = false;							// 追従するかどうか
	m_bRotationZ = false;						// Z回転出来るかどうか
	m_bRotationY = false;						// Y回転出来るかどうか
	m_bRockON = false;							// ロックオンするか
	m_state = CAMERASTATE_NONE;					// 状態
	m_nCntState = 0;							// 状態カウンター
	m_nCntDistance = 0;							// 距離カウンター
	m_nOriginCntDistance = 0;					// 元の距離カウンター
	m_fDistanceCorrection = 0.0f;				// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;			// 距離の減少係数
	m_nChasePlayerIndex = 0;					// 追従するプレイヤーのインデックス番号
	m_RockOnDir = ROCKON_DIR_RIGHT;				// ロックオン時の向き
	m_stateRockOn = ROCKON_NORMAL;				// ロックオン時の状態

	m_StateCameraR = POSR_STATE_NORMAL;		// 注視点の状態
	m_pStateCameraR = nullptr;	// 注視点の状態ポインタ
	m_pStateCameraV = nullptr;	// 視点の状態ポインタ
	m_pControlState = nullptr;	// 操作の状態ポインタ

}

//==========================================================================
// デストラクタ
//==========================================================================
CCamera::~CCamera()
{

}

//==========================================================================
// カメラの初期化処理
//==========================================================================
HRESULT CCamera::Init()
{
	
	// ビューポートの設定
	SetViewPort(MyLib::Vector3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(SCREEN_WIDTH, SCREEN_HEIGHT));

	// リセット
	m_bFollow = true;	// 追従するかどうか
	Reset(CScene::MODE_GAME);

	// 視点の代入処理
	SetCameraV();

	// 注視点の状態設定
	SetStateCamraR(DEBUG_NEW CStateCameraR());

	// 視点の状態設定
	SetStateCameraV(DEBUG_NEW CStateCameraV());

	// 操作の状態設定
	SetControlState(DEBUG_NEW CCameraControlState_Normal(this));

	return S_OK;
}

//==========================================================================
// ビューポートの設定
//==========================================================================
void CCamera::SetViewPort(MyLib::Vector3 pos, D3DXVECTOR2 size)
{
	m_viewport.X = (DWORD)pos.x;			// 描画する画面の左上X座標
	m_viewport.Y = (DWORD)pos.y;			// 描画する画面の左上Y座標
	m_viewport.Width = (DWORD)size.x;		// 描画する画面の幅
	m_viewport.Height = (DWORD)size.y;		// 描画する画面の高さ
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 1.0f;
}

//==========================================================================
// カメラの終了処理
//==========================================================================
void CCamera::Uninit()
{
	if (m_pStateCameraR != nullptr)
	{
		delete m_pStateCameraR;
		m_pStateCameraR = nullptr;
	}

	// 視点の状態ポインタ
	if (m_pStateCameraV != nullptr)
	{
		delete m_pStateCameraV;
		m_pStateCameraV = nullptr;
	}

	if (m_pControlState != nullptr)
	{
		delete m_pControlState;
		m_pControlState = nullptr;
	}
}

//==========================================================================
// カメラの更新処理
//==========================================================================
void CCamera::Update()
{
	// キーボード情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 高さの差分リセット
	m_fDiffHeightSave = 0.0f;

	// 視点・注視点移動
	MoveCameraFollow();
	MoveCameraInput();
	MoveCameraVR();
	MoveCameraR();
	MoveCameraV();
	MoveCameraDistance();
	MoveCameraDistance();
	UpdateSpotLightVec();

	// 状態更新
	UpdateState();
	

	//#ifdef _DEBUG

	if (pInputKeyboard->GetTrigger(DIK_F7) == true)
	{// F7が押された,追従切り替え

		m_bFollow = m_bFollow ? false : true;
	}

	if (pInputKeyboard->GetTrigger(DIK_F5) == true)
	{
		m_move = MyLib::Vector3(0.0f, 0.0f, 0.0f);					// 移動量
		m_rot = MyLib::Vector3(m_rot.x, 0.0f, m_rot.z);					// 向き
		m_rotVDest = m_rot;										// 目標の視点の向き
		m_fOriginDistance = 0.0f;					// 元の距離
		m_fDestDistance = START_CAMERALEN;
		m_fDistance = m_fDestDistance;
	}
	//#endif

		// テキストの描画
	CManager::GetInstance()->GetDebugProc()->Print(
		"---------------- カメラ情報 ----------------\n"
		"【向き】[X：%f Y：%f Z：%f]\n"
		"【距離】[%f]\n"
		"【視点】[X：%f Y：%f Z：%f]\n"
		"【注視点】[X：%f Y：%f Z：%f]\n",
		m_rot.x, m_rot.y, m_rot.z,
		m_fDistance,
		m_posV.x, m_posV.y, m_posV.z,
		m_posR.x, m_posR.y, m_posR.z);
}

//==========================================================================
// モード別更新処理
//==========================================================================
void CCamera::UpdateByMode()
{
	switch (CManager::GetInstance()->GetMode())
	{
	case CScene::MODE_TITLE:
		break;

	case CScene::MODE_GAME:
		break;

	default:
		break;
	}
}

//==========================================================================
// 入力機器のカメラ移動
//==========================================================================
void CCamera::MoveCameraInput()
{
	// マウスでの移動処理
	MoveCameraMouse();


	// スティックでの移動処理
	MoveCameraStick();
}

//==========================================================================
// コントローラーでの移動処理
//==========================================================================
void CCamera::MoveCameraStick(int nIdx)
{
	if (CManager::GetInstance()->GetMode() == CScene::MODE::MODE_GAME ||
		CManager::GetInstance()->GetMode() == CScene::MODE::MODE_GAMETUTORIAL)
	{
		// 操作処理
		m_pControlState->MoveCamera(this);
	}

	// 角度の正規化
	UtilFunc::Transformation::RotNormalize(m_rot);

	// 視点の代入処理
	SetCameraV();
}

//==========================================================================
// マウスでの移動処理
//==========================================================================
void CCamera::MoveCameraMouse()
{
	if (CManager::GetInstance()->GetMode() == CScene::MODE::MODE_GAME ||
		CManager::GetInstance()->GetMode() == CScene::MODE::MODE_GAMETUTORIAL)
	{
		// キーボード情報取得
		CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

		// キーボード情報取得
		CInputMouse* pInputMouse = CManager::GetInstance()->GetInputMouse();

		if (pInputMouse->GetPress(CInputMouse::BUTTON_LEFT) == true &&
			pInputMouse->GetPress(CInputMouse::BUTTON_RIGHT) == true)
		{// 左右同時押し

	//#if _DEBUG
			m_move.x += (pInputMouse->GetMouseMove().x * sinf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE) -
				(pInputMouse->GetMouseMove().y * cosf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE);

			m_move.z += (pInputMouse->GetMouseMove().x * cosf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE) +
				(pInputMouse->GetMouseMove().y * sinf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE);
			//#endif

					// 移動量補正
			MoveCameraVR();

			// 角度の正規化
			UtilFunc::Transformation::RotNormalize(m_rot.y);
			UtilFunc::Transformation::RotNormalize(m_rot.z);

			// 注視点設定
			SetCameraR();
		}
		else if (pInputMouse->GetPress(CInputMouse::BUTTON_LEFT) == true)
		{// 左クリックしてるとき,視点回転

			m_rot.y += pInputMouse->GetMouseMove().x * ROT_MOVE_MOUSE;

			//#if _DEBUG

			m_rot.z += pInputMouse->GetMouseMove().y * ROT_MOVE_MOUSE;
			//#endif

					// 角度の正規化
			UtilFunc::Transformation::RotNormalize(m_rot.y);
			UtilFunc::Transformation::RotNormalize(m_rot.z);

			// 値の正規化
			UtilFunc::Transformation::ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

			// 視点の代入処理
			SetCameraV();

		}
		else if (pInputMouse->GetPress(CInputMouse::BUTTON_RIGHT) == true)
		{// 右クリックしてるとき,注視点回転

			m_rot.y += pInputMouse->GetMouseMove().x * ROT_MOVE_MOUSE;

			//#if _DEBUG
			m_rot.z += pInputMouse->GetMouseMove().y * ROT_MOVE_MOUSE;
			//#endif

					// 角度の正規化
			UtilFunc::Transformation::RotNormalize(m_rot.y);
			UtilFunc::Transformation::RotNormalize(m_rot.z);

			// 値の正規化
			UtilFunc::Transformation::ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

			// 注視点の位置更新
			SetCameraR();
		}

		// マウスホイールで距離調整
		m_fDistance += pInputMouse->GetMouseMove().z * (MOVE * 0.3f);
		m_fDestDistance += pInputMouse->GetMouseMove().z * (MOVE * 0.3f);
		m_fOriginDistance += pInputMouse->GetMouseMove().z * (MOVE * 0.3f);
	}

	// 視点の代入処理
	SetCameraV();
}

//==========================================================================
// カメラの視点移動
//==========================================================================
void CCamera::MoveCameraV()
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

#ifdef _DEBUG
	// 視点移動
	if (pInputKeyboard->GetPress(DIK_Y) == true)
	{// Yキーが押された,視点上移動

		m_rot.z -= ROT_MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_N) == true)
	{// Nキーが押された,視点下移動

		m_rot.z += ROT_MOVE;
	}

	if (pInputKeyboard->GetPress(DIK_Z) == true)
	{// Zキーが押された

		m_rot.y += ROT_MOVE;
	}
	if (pInputKeyboard->GetPress(DIK_C) == true)
	{// Cキーが押された

		m_rot.y -= ROT_MOVE;
	}
#endif


	// 角度の正規化
	UtilFunc::Transformation::RotNormalize(m_rot.y);
	UtilFunc::Transformation::RotNormalize(m_rot.z);

	// 値の正規化
	UtilFunc::Transformation::ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

	// 視点の代入処理
	SetCameraV();

}

//==========================================================================
// カメラの注視点移動
//==========================================================================
void CCamera::MoveCameraR()
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

#ifdef _DEBUG
	// 旋回
	if (pInputKeyboard->GetPress(DIK_Q) == true)
	{// Qキーが押された,左旋回


		m_rot.y -= ROT_MOVE;

	}
	else if (pInputKeyboard->GetPress(DIK_E) == true)
	{// Eキーが押された,左旋回

		m_rot.y += ROT_MOVE;
	}

	// 上昇下降
	if (pInputKeyboard->GetPress(DIK_T) == true)
	{// Tキーが押された

		m_rot.z += ROT_MOVE;

	}
	else if (pInputKeyboard->GetPress(DIK_B) == true)
	{// Bキーが押された

		m_rot.z -= ROT_MOVE;

	}
#endif


	// 角度の正規化
	UtilFunc::Transformation::RotNormalize(m_rot.y);
	UtilFunc::Transformation::RotNormalize(m_rot.z);

	// 値の正規化
	UtilFunc::Transformation::ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

	// 注視点の位置更新
	SetCameraR();
}

//==========================================================================
// カメラの視点・注視点移動
//==========================================================================
void CCamera::MoveCameraVR()
{

	// 移動量分を加算
	m_posV.x += m_move.x;
	m_posV.z += m_move.z;

	// 移動量をリセット
	m_move.x = 0.0f;
	m_move.z = 0.0f;

	// 注視点の位置更新
	SetCameraR();
}

//==========================================================================
//  視点・注視点間距離
//==========================================================================
void CCamera::MoveCameraDistance()
{
	// 距離カウンター減算
	m_nCntDistance--;

	if (m_nCntDistance <= 0)
	{
		m_nCntDistance = 0;

		if (m_fDestDistance >= m_fOriginDistance)
		{// カメラを近づけていく
			m_fDestDistance -= m_fDistanceDecrementValue;
		}

		if (m_fDestDistance <= m_fOriginDistance)
		{// 補正しすぎたら戻す
			m_fDestDistance = m_fOriginDistance;
		}
	}

	// 限界値補正
	if (m_fDistance >= MAX_LENGTH)
	{
		m_fDistance = MAX_LENGTH;
	}
	else if (m_fDistance <= MIN_LENGTH)
	{
		m_fDistance = MIN_LENGTH;
	}

	// 目標の距離へ補正
	UtilFunc::Correction::InertiaCorrection(m_fDistance, m_fDestDistance, m_fDistanceCorrection);

}

//==========================================================================
// 通常状態のロックオン処理
//==========================================================================
void CCamera::RockOnStateNormal()
{
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nChasePlayerIndex);
	MyLib::Vector3 playerpos = pPlayer->GetPosition();

	// 目標地点をロックオンとの中心にする
	m_TargetPosDest = UtilFunc::Calculation::GetCenterPosition3D(m_RockOnPos, playerpos);

	// 慣性補正
	float factor = 0.2f;
	UtilFunc::Correction::InertiaCorrection(m_TargetPos.x, m_TargetPosDest.x, factor);
	UtilFunc::Correction::InertiaCorrection(m_TargetPos.y, m_TargetPosDest.y, factor);
	UtilFunc::Correction::InertiaCorrection(m_TargetPos.z, m_TargetPosDest.z, factor);

	// 2点間の距離
	float fLen = UtilFunc::Calculation::GetFabsPosLength3D(m_RockOnPos, playerpos);
	float ratio = fLen / CGame::GetInstance()->GetRockOnDistance();

	// 最大ロックオン距離
	float maxRockOnDistance = CGame::GetInstance()->GetRockOnDistance();

	if (ratio <= 1.0f)
	{
		// 目標の長さ設定
		float setDistance = maxRockOnDistance * ratio;
		if (setDistance <= m_fRockOnDistance)
		{
			setDistance = m_fRockOnDistance;
		}

		SetLenDest(setDistance, 2, 2.0f, 0.1f);

		// 目標の角度を求める
		m_rotDest.y =
			D3DX_PI +
			atan2f((m_TargetPos.x - m_RockOnPos.x), (m_TargetPos.z - m_RockOnPos.z)) +
			((ROTDISTANCE_ROCKON * 2.0f) * (m_RockOnDir - 1)) + ROTDISTANCE_ROCKON;

		if (m_RockOnPos.y > 150.0f)
		{
			// 目標の角度を求める
			m_rotDest.z = atan2f((m_RockOnPos.y - 150.0f), setDistance) + DEFAULT_GAMEROT.z;
		}
		else
		{
			m_rotDest.z = DEFAULT_GAMEROT.z;
		}

		UtilFunc::Transformation::RotNormalize(m_rotDest);

		// 目標の向き
		float fRotDiff = m_rotDest.y - m_rot.y;
		UtilFunc::Transformation::RotNormalize(fRotDiff);
		m_rot.y += fRotDiff * 0.08f;

		fRotDiff = m_rotDest.z - m_rot.z;
		UtilFunc::Transformation::RotNormalize(fRotDiff);
		m_rot.z += fRotDiff * 0.08f;
	}
	else
	{
		// 通常状態
		SetStateCamraR(DEBUG_NEW CStateCameraR());

		m_bRockON = false;

		// 目標の長さ設定
		SetLenDest(maxRockOnDistance * 0.5f, 60, 2.0f, 0.1f);
	}


	// テキストの描画
	CManager::GetInstance()->GetDebugProc()->Print(
		"---------------- カメラ情報 ----------------\n"
		"【目標の向き】[X：%f Y：%f Z：%f]\n",
		m_rotDest.x, m_rotDest.y, m_rotDest.z);
}

//==========================================================================
// カウンター状態のロックオン処理
//==========================================================================
void CCamera::RockOnStateCounter()
{
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nChasePlayerIndex);
	MyLib::Vector3 playerpos = pPlayer->GetPosition();

	// 目標地点をロックオンとの中心にする
	m_TargetPosDest = playerpos;

	// 慣性補正
	UtilFunc::Correction::InertiaCorrection(m_TargetPos.x, m_TargetPosDest.x, 0.25f);
	UtilFunc::Correction::InertiaCorrection(m_TargetPos.y, m_TargetPosDest.y, 0.25f);
	UtilFunc::Correction::InertiaCorrection(m_TargetPos.z, m_TargetPosDest.z, 0.25f);

	// 目標の向き
	float fRotDiff = m_rotDest.y - m_rot.y;
	UtilFunc::Transformation::RotNormalize(fRotDiff);

	m_rot.y += fRotDiff * 0.08f;
	UtilFunc::Transformation::RotNormalize(m_rot.y);
}

//==========================================================================
// カメラの視点代入処理
//==========================================================================
void CCamera::SetCameraV()
{

	switch (CManager::GetInstance()->GetMode())
	{
	case CScene::MODE_TITLE:
		SetCameraVTitle();
		break;

	case CScene::MODE_TUTORIAL:
		SetCameraVGame();
		break;

	case CScene::MODE_GAME:
	case CScene::MODE_GAMETUTORIAL:
		SetCameraVGame();
		break;

	case CScene::MODE_RESULT:
		SetCameraVResult();
		break;

	case CScene::MODE_RANKING:
		SetCameraVRanking();
		SetCameraVTitle();
		break;

	default:
		SetCameraVResult();
		break;
	}

}

//==========================================================================
// カメラの視点代入処理(タイトル)
//==========================================================================
void CCamera::SetCameraVTitle()
{
	if (m_bFollow == false)
	{// 追従しないとき

		// 視点の代入処理
		m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	}
	else if (m_bFollow == true)
	{// 追従ON

		// 視点の代入処理
		m_posVDest.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
		m_posVDest.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
		m_posVDest.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;

		// 補正する
		m_posV += (m_posVDest - m_posV) * 0.12f;
	}
}

//==========================================================================
// カメラの視点代入処理(ゲーム)
//==========================================================================
void CCamera::SetCameraVGame()
{
	// Y回転のフラグ
	m_bRotationY = true;

	if (m_bFollow == false)
	{// 追従しないとき

		// 視点の代入処理
		m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	}
	else if (m_bFollow == true)
	{// 追従ON

		// 視点の代入処理
		m_posVDest.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
		m_posVDest.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
		m_posVDest.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;

		// 位置制限
		m_pStateCameraV->LimitPos(this);

		float fDistance = 0.0f;
		m_fHeightMaxDest = m_posVDest.y;

		// Z操作出来る状態
		m_bRotationZ = true;

		// 目標の角度を求める
		float fRotDest = atan2f((m_posVDest.x - m_posR.x), (m_posVDest.z - m_posR.z));
		while (1)
		{
			// 仮想の弾の位置
			float fPosBulletX = m_TargetPos.x + cosf(m_rot.z) * sinf(m_rot.y) * -fDistance;
			float fPosBulletZ = m_TargetPos.z + cosf(m_rot.z) * cosf(m_rot.y) * -fDistance;

			// 高さ取得
			bool bLand = false;
			CElevation* pElevation = CGame::GetInstance()->GetElevation();
			if (pElevation == nullptr)
			{
				return;
			}

			float fHeight = pElevation->GetHeight(MyLib::Vector3(fPosBulletX, 0.0f, fPosBulletZ), &bLand);

			if (m_fHeightMaxDest <= fHeight)
			{// 最大の高さを更新したら

				// 距離の応じた割合保存
				float fDistanceRatio = fDistance / (m_fDistance);

				// 目標の最大高さ保存
				m_fHeightMaxDest = fHeight * (fDistanceRatio + 1.0f);

				// Z操作不能
				m_bRotationZ = false;
				m_Moverot.z = 0.0f;
			}

			// 長さ加算
			fDistance += 10.0f;

			if (fDistance >= m_fDistance)
			{// 長さを超えたら終わり
				break;
			}
		}

		// 目標の視点更新
		if (m_fHeightMaxDest > m_posVDest.y)
		{
			// 最大の高さ補正
			m_fHeightMax += (m_fHeightMaxDest - m_fHeightMax) * 0.08f;

			m_posVDest.y = m_fHeightMax;

			// 高さの差分
			m_fDiffHeightSave += m_fHeightMax - m_posV.y;
		}

		// 補正する
		m_posV += (m_posVDest - m_posV) * (0.12f * MULTIPLY_POSV_CORRECTION);
	}
}

//==========================================================================
// カメラの視点代入処理(リザルト)
//==========================================================================
void CCamera::SetCameraVResult()
{
	// 視点の代入処理
	m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
}

//==========================================================================
// カメラの視点代入処理(ランキング)
//==========================================================================
void CCamera::SetCameraVRanking()
{
	// 視点の代入処理
	m_posV = MyLib::Vector3(RANKING_POS_V.x, RANKING_POS_V.y, RANKING_POS_V.z);
}

//==========================================================================
// カメラの注視点代入処理
//==========================================================================
void CCamera::SetCameraR()
{

	switch (CManager::GetInstance()->GetMode())
	{
	case CScene::MODE_TITLE:
		SetCameraRTitle();
		break;

	case CScene::MODE_TUTORIAL:
		SetCameraRGame();
		break;

	case CScene::MODE_GAME:
	case CScene::MODE_GAMETUTORIAL:
		SetCameraRGame();
		break;

	case CScene::MODE_RESULT:
		SetCameraRResult();
		break;

	case CScene::MODE_RANKING:
		SetCameraRRanking();
		SetCameraRTitle();
		break;

	default:
		SetCameraRResult();
		break;
	}

}

//==========================================================================
// カメラの注視点代入処理(タイトル)
//==========================================================================
void CCamera::SetCameraRTitle()
{
	if (m_bFollow == false)
	{// 追従しないとき

		// 注視点の代入処理
		m_posR.x = m_posV.x + cosf(m_rot.z) * sinf(m_rot.y) * m_fDistance;
		m_posR.z = m_posV.z + cosf(m_rot.z) * cosf(m_rot.y) * m_fDistance;
		m_posR.y = m_posV.y + sinf(m_rot.z) * m_fDistance;
	}
	else
	{// 追従ON

		// 注視点の代入処理
		m_posRDest = m_TargetPos;

		// 補正する
		m_posR += (m_posRDest - m_posR) * 0.08f;
	}
}

//==========================================================================
// カメラの注視点代入処理(ゲーム)
//==========================================================================
void CCamera::SetCameraRGame()
{
	if (m_bFollow == false)
	{// 追従しないとき

		// 注視点の代入処理
		m_posR.x = m_posV.x + cosf(m_rot.z) * sinf(m_rot.y) * m_fDistance;
		m_posR.z = m_posV.z + cosf(m_rot.z) * cosf(m_rot.y) * m_fDistance;
		m_posR.y = m_posV.y + sinf(m_rot.z) * m_fDistance;
	}
	else
	{// 追従ON

		float fYcamera = 100.0f;
		float fCorrectionHeight = 50.0f;

		if (m_TargetPos.y >= fCorrectionHeight)
		{
			fYcamera = (m_TargetPos.y - fCorrectionHeight) + fYcamera;
		}
		else if (m_TargetPos.y <= -fCorrectionHeight)
		{
			fYcamera = (m_TargetPos.y + fCorrectionHeight) + fYcamera;
		}

		fYcamera = m_TargetPos.y + 150.0f;

		if (fYcamera <= 0.0f)
		{
			m_fDiffHeightSave += -fYcamera;
		}

		// 目標の高さの差分を代入
		m_fDiffHeightDest = m_fDiffHeightSave;

		// 高さの差分を補正する
		m_fDiffHeight += (m_fDiffHeightDest - m_fDiffHeight) * 0.01f;

		// 注視点の代入処理
		m_pStateCameraR->SetCameraR(this);
		m_posRDest.y = fYcamera - m_fDiffHeight;

		// 補正する
		m_posR += (m_posRDest - m_posR) * (0.08f * MULTIPLY_POSR_CORRECTION);
	}
}

//==========================================================================
// カメラの注視点代入処理(リザルト)
//==========================================================================
void CCamera::SetCameraRResult()
{
	// 注視点の代入処理
	m_posR.x = m_posV.x + cosf(m_rot.z) * sinf(m_rot.y) * m_fDistance;
	m_posR.z = m_posV.z + cosf(m_rot.z) * cosf(m_rot.y) * m_fDistance;
	m_posR.y = m_posV.y + sinf(m_rot.z) * m_fDistance;
}

//==========================================================================
// カメラの注視点代入処理(ランキング)
//==========================================================================
void CCamera::SetCameraRRanking()
{
	// 注視点の代入処理
	m_posR.x = m_posV.x + cosf(m_rot.z) * sinf(m_rot.y) * m_fDistance;
	m_posR.z = m_posV.z + cosf(m_rot.z) * cosf(m_rot.y) * m_fDistance;
	m_posR.y = m_posV.y + sinf(m_rot.z) * m_fDistance;
}

//==========================================================================
// カメラの背面自動追従
//==========================================================================
void CCamera::MoveCameraFollow()
{

}

//==========================================================================
// カメラの設定処理
//==========================================================================
void CCamera::SetCamera()
{

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// ビューポートの設定
	pDevice->SetViewport(&m_viewport);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
								D3DXToRadian(45.0f),	// 視野角
								(float)m_viewport.Width / (float)m_viewport.Height,	// アスペクト比
								10.0f,		// 手前の制限
								30000.0f);	// 奥行きの制限

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,	// 視点
		&m_posR,	// 注視点
		&m_vecU);	// 上方向ベクトル

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

}

//==========================================================================
// スポットライトのベクトル更新
//==========================================================================
void CCamera::UpdateSpotLightVec()
{
	// 方向ベクトル
	MyLib::Vector3 vec = mylib_const::DEFAULT_VECTOR3;

	// 視点から注視点への向き
	vec = m_posR - m_posV;

	// 正規化
	D3DXVec3Normalize(&vec, &vec);

	// スポットライトの方向設定
	CManager::GetInstance()->GetLight()->UpdateSpotLightDirection(vec);
}

//==========================================================================
// 目標の長さ設定
//==========================================================================
void CCamera::SetLenDest(float fLength, int nCntTime, float DecrementValue, float fCorrection)
{
	// 目標の距離設定
	m_fDestDistance = fLength;
	m_nCntDistance = nCntTime;
	m_nOriginCntDistance = m_nCntDistance;		// 元の距離カウンター
	m_fDistanceCorrection = fCorrection;		// 減少補正係数
	m_fDistanceDecrementValue = DecrementValue;	// 減少量
}

//==========================================================================
// カメラの振動設定処理
//==========================================================================
void CCamera::SetShake(int nTime, float fLength, float fLengthY)
{
	// 振動状態に設定
	m_state = CAMERASTATE_SHAKE;

	if (m_nCntState > 0)
	{
		if (m_nShakeLength <= fLength)
		{
			m_nShakeLength = fLength;	// 揺れの大きさ
		}
		if (m_nShakeLengthY <= fLengthY)
		{
			m_nShakeLengthY = fLengthY;	// 揺れの大きさ
		}
		if (m_nCntState <= nTime)
		{
			m_nCntState = nTime;	// 状態遷移カウンター
		}
	}
	else
	{
		m_nShakeLength = fLength;	// 揺れの大きさ
		m_nShakeLengthY = fLengthY;	// Yの揺れの大きさ
		m_nCntState = nTime;		// 状態遷移カウンター
	}
}

//==========================================================================
// カメラの状態更新処理
//==========================================================================
void CCamera::UpdateState()
{
	switch (m_state)
	{
	case CAMERASTATE_NONE:
		break;

	case CAMERASTATE_SHAKE:

		if (m_nCntState % 3 == 0)
		{
			// 画面揺れ
			Shake();
		}
		else
		{
			// 視点の代入処理
			m_posV.x += m_fMoveShake;
			m_posV.y += m_fMoveShakeY;
			m_posV.z += m_fMoveShake;

			// 注視点の代入処理
			m_posR.x += m_fMoveShake;
			m_posR.y += m_fMoveShakeY;
			m_posR.z += m_fMoveShake;

			// 慣性補正
			UtilFunc::Correction::InertiaCorrection(m_nShakeLength, 0.0f, 0.01f);
			UtilFunc::Correction::InertiaCorrection(m_nShakeLengthY, 0.0f, 0.01f);
		}

		// 状態遷移カウンター減算
		m_nCntState--;

		if (m_nCntState <= 0)
		{
			m_nCntState = 0;
			m_state = CAMERASTATE_NONE;
			m_nShakeLength = 0.0f;	// 揺れの大きさ
			m_nShakeLengthY = 0.0f;	// Yの揺れの大きさ
			switch (CManager::GetInstance()->GetMode())
			{
			case CScene::MODE_RESULT:
				ResetResult();
				break;
			}
		}
		break;

	case CAMERASTATE_PRAYER:
		StatePrayer();
		break;
	}
}

//==========================================================================
// カメラの振動処理
//==========================================================================
void CCamera::Shake()
{
	// 長さ取得
	int nLength = (int)m_nShakeLength;
	int nLengthY = (int)m_nShakeLengthY;

	m_fMoveShake = (float)UtilFunc::Transformation::Random(-nLength, nLength) * sinf((float)UtilFunc::Transformation::Random(-314, 314) * 0.01f);	// 揺れの移動量
	m_fMoveShakeY = (float)UtilFunc::Transformation::Random(-nLengthY, nLengthY);						// Yの揺れの移動量

	// 視点の代入処理
	m_posV.x += (float)UtilFunc::Transformation::Random(-nLength, nLength) * sinf((float)UtilFunc::Transformation::Random(-314, 314) * 0.01f);
	m_posV.y += (float)UtilFunc::Transformation::Random(-nLengthY, nLengthY);
	m_posV.z += (float)UtilFunc::Transformation::Random(-nLength, nLength) * cosf((float)UtilFunc::Transformation::Random(-314, 314) * 0.01f);

	// 注視点の代入処理
	m_posR.x += (float)UtilFunc::Transformation::Random(-nLength, nLength) * sinf((float)UtilFunc::Transformation::Random(-314, 314) * 0.01f);
	m_posR.y += (float)UtilFunc::Transformation::Random(-nLengthY, nLengthY);
	m_posR.z += (float)UtilFunc::Transformation::Random(-nLength, nLength) * cosf((float)UtilFunc::Transformation::Random(-314, 314) * 0.01f);

	// 慣性補正
	UtilFunc::Correction::InertiaCorrection(m_nShakeLength, 0.0f, 0.1f);
	UtilFunc::Correction::InertiaCorrection(m_nShakeLengthY, 0.0f, 0.1f);
}

//==========================================================================
// 祈り更新処理
//==========================================================================
void CCamera::StatePrayer()
{
	
}

//==========================================================================
// リセット
//==========================================================================
void CCamera::Reset(CScene::MODE mode)
{
	// 通常状態
	SetStateCamraR(DEBUG_NEW CStateCameraR());
	SetStateCameraV(DEBUG_NEW CStateCameraV());

	// 操作の状態設定
	SetControlState(DEBUG_NEW CCameraControlState_Normal(this));

	m_bFollow = true;	// 追従するかどうか
	m_bRockON = false;	// ロックオンするか

	switch (mode)
	{
	case CScene::MODE_NONE:
		break;

	case CScene::MODE_TITLE:
		ResetTitle();
		break;

	case CScene::MODE_TUTORIAL:
		ResetGame();
		break;

	case CScene::MODE_GAME:
	case CScene::MODE_GAMETUTORIAL:
		ResetGame();
		break;

	case CScene::MODE_RESULT:
		ResetResult();
		break;

	case CScene::MODE_RANKING:
		ResetRanking();
		break;

	case CScene::MODE_MAX:
		break;

	default:
		break;
	}
	
	// プロジェクションマトリックスの初期化
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection, D3DXToRadian(45.0f),
								(float)m_viewport.Width / (float)m_viewport.Height,
								10.0f,		// 奥行きの制限
								30000.0f);	// 奥行きの制限

	// ビューマトリックスの初期化
	D3DXMatrixLookAtLH(&m_mtxView, &m_posV, &m_posR, &m_vecU);

	// 視点の代入処理
	SetCameraV();
}

//==========================================================================
// ゲームのリセット
//==========================================================================
void CCamera::ResetGame()
{
	m_posR = MyLib::Vector3(0.0f, 200.0f, 0.0f);				// 注視点(見たい場所)
	m_posV = MyLib::Vector3(0.0f, 300.0f, m_posR.z + -400.0f);	// 視点(カメラの位置)
	m_posVDest = m_posV;									// 目標の視点
	m_posRDest = m_posR;									// 目標の注視点
	m_vecU = MyLib::Vector3(0.0f, 1.0f, 0.0f);					// 上方向ベクトル
	m_move = MyLib::Vector3(0.0f, 0.0f, 0.0f);					// 移動量
	m_rot = DEFAULT_GAMEROT;					// 向き
	m_rotDest = DEFAULT_GAMEROT;							// 目標の向き
	m_rotVDest = m_rot;										// 目標の視点の向き
	m_TargetPos = MyLib::Vector3(0.0f, 0.0f, 0.0f);			// 目標の位置
	m_fDistance = START_CAMERALEN;							// 距離
	m_fDestDistance = START_CAMERALEN;						// 目標の距離
	m_fOriginDistance = START_CAMERALEN;					// 元の距離
	m_fDiffHeight = 0.0f;									// 高さの差分
	m_fDiffHeightSave = 0.0f;								// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;								// 目標の高さの差分
	m_state = CAMERASTATE_NONE;								// 状態
	m_nCntState = 0;							// 状態カウンター
	m_nCntDistance = 0;							// 距離カウンター
	m_nOriginCntDistance = 0;					// 元の距離カウンター
	m_fDistanceCorrection = 0;					// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;			// 距離の減少係数
	m_fHeightMaxDest = 0.0f;					// カメラの最大高さの目標
}

//==========================================================================
// リセット
//==========================================================================
void CCamera::ResetBoss()
{
	ResetGame();
}

//==========================================================================
//  スクリーン内の判定
//==========================================================================
bool CCamera::IsOnScreen(const MyLib::Vector3 pos)
{
	// 返り値用の変数
	bool bIn = false;

	// 判定
	//ビューポートの設定
	D3DVIEWPORT9 vp = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 1.0f };

	//計算用変数宣言
	D3DXMATRIX mtxWorld; //ワールドマトリックス

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	//敵のスクリーン座標を算出
	MyLib::Vector3 screenPos;
	D3DXVec3Project
	(
		&screenPos,
		&pos,
		&vp,
		&m_mtxProjection,
		&m_mtxView,
		&mtxWorld
	);

	// 判定
	if (screenPos.x >= 0.0f && screenPos.x <= SCREEN_WIDTH &&
		screenPos.y >= 0.0f && screenPos.y <= SCREEN_HEIGHT &&
		screenPos.z < 1.0f)
	{
		bIn = true;
	}

	// 返す
	return bIn;
}

//==========================================================================
// タイトルのリセット
//==========================================================================
void CCamera::ResetTitle()
{
	m_posV = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 視点(カメラの位置)
	m_posR = TITLE_POSR_DEST;	// 注視点(見たい場所)
	m_posVDest = m_posV;								// 目標の視点
	m_posRDest = m_posR;								// 目標の注視点
	m_vecU = MyLib::Vector3(0.0f, 1.0f, 0.0f);				// 上方向ベクトル
	m_move = MyLib::Vector3(0.0f, 0.0f, 0.0f);				// 移動量
	m_rot = TITLECAMERAROT_NONE;						// 向き
	m_rotVDest = m_rot;									// 目標の視点の向き
	m_TargetPos = m_posR;								// 目標の位置
	m_fDistance = TITLE_LEN_DEST;						// 距離
	m_fDestDistance = TITLE_LEN_DEST;					// 目標の距離
	m_fOriginDistance = TITLE_LEN_DEST;					// 元の距離
	m_fDiffHeight = 0.0f;								// 高さの差分
	m_fDiffHeightSave = 0.0f;							// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;							// 目標の高さの差分
	m_state = CAMERASTATE_NONE;							// 状態
	m_nCntState = 0;									// 状態カウンター
	m_nCntDistance = 0;									// 距離カウンター
	m_nOriginCntDistance = 0;							// 元の距離カウンター
	m_fDistanceCorrection = 0;							// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;					// 距離の減少係数
	m_fHeightMaxDest = 0.0f;							// カメラの最大高さの目標
	m_bFollow = true;									// 追従フラグ
}

//==========================================================================
// リザルトリセット
//==========================================================================
void CCamera::ResetResult()
{
	m_posR = MyLib::Vector3(0.0f, 300.0f, 30.0f);	// 注視点(見たい場所)
	m_posV = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 視点(カメラの位置)
	m_posVDest = m_posV;								// 目標の視点
	m_posRDest = m_posR;								// 目標の注視点
	m_vecU = MyLib::Vector3(0.0f, 1.0f, 0.0f);				// 上方向ベクトル
	m_move = MyLib::Vector3(0.0f, 0.0f, 0.0f);				// 移動量
	m_rot = TITLECAMERAROT_NONE;						// 向き
	m_rotVDest = m_rot;									// 目標の視点の向き
	m_TargetPos = m_posR;								// 目標の位置
	m_fDistance = TITLE_LEN_DEST;						// 距離
	m_fDestDistance = TITLE_LEN_DEST;					// 目標の距離
	m_fOriginDistance = TITLE_LEN_DEST;					// 元の距離
	m_fDiffHeight = 0.0f;								// 高さの差分
	m_fDiffHeightSave = 0.0f;							// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;							// 目標の高さの差分
	m_state = CAMERASTATE_NONE;							// 状態
	m_nCntState = 0;									// 状態カウンター
	m_nCntDistance = 0;									// 距離カウンター
	m_nOriginCntDistance = 0;							// 元の距離カウンター
	m_fDistanceCorrection = 0;							// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;					// 距離の減少係数
	m_fHeightMaxDest = 0.0f;							// カメラの最大高さの目標
	m_bFollow = true;									// 追従フラグ
}

//==========================================================================
// ランキングのリセット
//==========================================================================
void CCamera::ResetRanking()
{
	m_posR = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 注視点(見たい場所)
	m_posV = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 視点(カメラの位置)
	m_posVDest = m_posV;								// 目標の視点
	m_posRDest = m_posR;								// 目標の注視点
	m_vecU = MyLib::Vector3(0.0f, 1.0f, 0.0f);				// 上方向ベクトル
	m_move = MyLib::Vector3(0.0f, 0.0f, 0.0f);				// 移動量
	m_TargetPos = MyLib::Vector3(0.0f, 0.0f, 0.0f);		// 目標の位置
	m_fDistance = RANKING_LEN_DEST;						// 距離
	m_fDestDistance = RANKING_LEN_DEST;					// 目標の距離
	m_fOriginDistance = RANKING_LEN_DEST;					// 元の距離
	m_fDiffHeight = 0.0f;								// 高さの差分
	m_fDiffHeightSave = 0.0f;							// 高さの差分保存用
	m_fDiffHeightDest = 0.0f;							// 目標の高さの差分
	m_state = CAMERASTATE_NONE;							// 状態
	m_nCntState = 0;									// 状態カウンター
	m_nCntDistance = 0;									// 距離カウンター
	m_nOriginCntDistance = 0;							// 元の距離カウンター
	m_fDistanceCorrection = 0;							// 距離の慣性補正係数
	m_fDistanceDecrementValue = 0.0f;					// 距離の減少係数
	m_fHeightMaxDest = 0.0f;							// カメラの最大高さの目標

	m_rot = RANKINGROT_NONE;							// 向き
	m_rotVDest = m_rot;									// 目標の視点の向き

	// 注視点の代入
	m_posR = MyLib::Vector3(0.0f, 0.0f, 0.0f);			// 注視点(見たい場所)

	// 視点の代入
	m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	m_posVDest = m_posV;								// 目標の視点
	m_posRDest = m_posR;								// 目標の注視点
}

//==========================================================================
// カメラの位置取得
//==========================================================================
MyLib::Vector3 CCamera::GetPositionV() const
{
	return m_posV;
}

//==========================================================================
// カメラの注視点取得
//==========================================================================
MyLib::Vector3 CCamera::GetPositionR() const
{
	return m_posR;
}

//==========================================================================
// 向き設定
//==========================================================================
void CCamera::SetRotation(const MyLib::Vector3 rot)
{
	m_rot = rot;
}

//==========================================================================
// 向き取得
//==========================================================================
MyLib::Vector3 CCamera::GetRotation() const
{
	return m_rot;
}

//==========================================================================
// 目標の向き設定
//==========================================================================
void CCamera::SetDestRotation(const MyLib::Vector3& rot)
{
	m_rotDest = rot;
}

//==========================================================================
// 目標の向き取得
//==========================================================================
MyLib::Vector3 CCamera::GetDestRotation()
{
	return m_rotDest;
}

//==========================================================================
// 目標の位置設定
//==========================================================================
void CCamera::SetTargetPosition(const MyLib::Vector3 pos)
{
	// 目標の位置
	m_TargetPos = pos;
}

//==========================================================================
// 目標の位置取得
//==========================================================================
MyLib::Vector3 CCamera::GetTargetPosition()
{
	return m_TargetPos;
}

//==========================================================================
// ロックオンの位置設定
//==========================================================================
void CCamera::SetRockOnPosition(const MyLib::Vector3 pos)
{
	m_RockOnPos = pos;
}

//==========================================================================
// ロックオンの位置取得
//==========================================================================
MyLib::Vector3 CCamera::GetRockOnPosition()
{
	return m_RockOnPos;
}

//==========================================================================
// ロックオンの下限距離設定
//==========================================================================
void CCamera::SetRockOnDistance(const float distance)
{
	m_fRockOnDistance = distance;
}

//==========================================================================
// ロックオン設定
//==========================================================================
void CCamera::SetRockOn(const MyLib::Vector3 pos, bool bSet)
{
	if (bSet)
	{
		SetStateCamraR(DEBUG_NEW CStateCameraR_RockOn());
		SetControlState(DEBUG_NEW CCameraControlState_RockOn(this));
	}
	else
	{
		SetStateCamraR(DEBUG_NEW CStateCameraR());
		// 操作の状態設定
		SetControlState(DEBUG_NEW CCameraControlState_Normal(this));
	}

	m_RockOnPos = pos;
	m_bRockON = bSet;
}

//==========================================================================
// 追従目標の向き設定
//==========================================================================
void CCamera::SetTargetRotation(const MyLib::Vector3 rot)
{
	// 目標の向き
	m_TargetRot = rot;
}

//==========================================================================
// 追従目標の向き取得
//==========================================================================
MyLib::Vector3 CCamera::GetTargetRotation()
{
	return m_TargetRot;
}

//==========================================================================
// 元になるカメラの距離設定
//==========================================================================
void CCamera::SetOriginDistance(float fDistance)
{
	m_fOriginDistance = fDistance;
}

//==========================================================================
// 元になるカメラの距離取得
//==========================================================================
float CCamera::GetOriginDistance()
{
	return m_fOriginDistance;
}

//==========================================================================
// 追従するプレイヤーのインデックス番号設定
//==========================================================================
void CCamera::SetPlayerChaseIndex(int nIdx)
{
	m_nChasePlayerIndex = nIdx;
}

//==========================================================================
// 追従するプレイヤーのインデックス番号取得
//==========================================================================
int CCamera::GetPlayerChaseIndex()
{
	return m_nChasePlayerIndex;
}

//==========================================================================
// 追従の判定設定
//==========================================================================
void CCamera::SetEnableFollow(bool bFollow)
{
	m_bFollow = bFollow;
}

//==========================================================================
// 追従状態取得
//==========================================================================
bool CCamera::IsFollow()
{
	return m_bFollow;
}

//==========================================================================
// ロックオン状態設定
//==========================================================================
void CCamera::SetRockOnState(RockOnState state, float height)
{
	if (state == ROCKON_COUNTER)
	{
		// 目標の角度を求める
		m_rotDest.y =
			D3DX_PI +
			atan2f((m_TargetPos.x - m_RockOnPos.x), (m_TargetPos.z - m_RockOnPos.z)) +
			((ROTDISTANCE_ROCKON * 2.0f) * (m_RockOnDir - 1)) + ROTDISTANCE_ROCKON;
		UtilFunc::Transformation::RotNormalize(m_rotDest);
		
		m_rotDest.y -= ROTDISTANCE_COUNTER.y;
		m_rotDest.z = D3DX_PI * 0.02f;

		height -= 200.0f;
		if (height <= 0.0f) {
			height = 0.0f;
		}

		float len = LENGTH_COUNTER + height;

		m_fOriginDistance = len;	// 元の距離

		// 目標の長さ設定
		SetLenDest(len, 48, 4.0f, 0.25f);
	}
	else
	{
		m_rotDest.z = DEFAULT_GAMEROT.z;
		m_fOriginDistance = START_CAMERALEN;	// 元の距離
	}
	UtilFunc::Transformation::RotNormalize(m_rotDest);

	m_stateRockOn = state;
}

//==========================================================================
// 注視点の状態設定
//==========================================================================
void CCamera::SetStateCamraR(CStateCameraR* state)
{
	delete m_pStateCameraR;
	m_pStateCameraR = state;
}

//==========================================================================
// 視点の状態設定
//==========================================================================
void CCamera::SetStateCameraV(CStateCameraV* state)
{
	delete m_pStateCameraV;
	m_pStateCameraV = state;
}

//==========================================================================
// 操作の状態設定
//==========================================================================
void CCamera::SetControlState(CCameraControlState* state)
{
	delete m_pControlState;
	m_pControlState = state;
}

//==========================================================================
// 通常の注視点設定
//==========================================================================
void CStateCameraR::SetCameraR(CCamera* pCamera)
{
	MyLib::Vector3 targetpos = pCamera->GetTargetPosition();
	MyLib::Vector3 rot = pCamera->GetRotation();

	MyLib::Vector3 posdest;
	posdest.x = (targetpos.x + sinf(rot.y) * DISATNCE_POSR_PLAYER);
	posdest.z = (targetpos.z + cosf(rot.y) * DISATNCE_POSR_PLAYER);
	pCamera->SetPositionRDest(posdest);
}

//==========================================================================
// メインの位置制限
//==========================================================================
void CStateCameraV::LimitPos(CCamera* pCamera)
{
	MyLib::Vector3 posVDest = pCamera->GetPositionVDest();

	if (posVDest.LengthXZ() > mylib_const::RADIUS_STAGE)
	{// 補正
		posVDest = posVDest.Normal() * mylib_const::RADIUS_STAGE;
	}
	if (posVDest.LengthXZ() > mylib_const::RADIUS_STAGE)
	{// 補正
		posVDest = posVDest.Normal() * mylib_const::RADIUS_STAGE;
	}

	pCamera->SetPositionVDest(posVDest);
}

//==========================================================================
// 強化ステージの位置制限
//==========================================================================
void CStateCameraV_Enhance::LimitPos(CCamera* pCamera)
{
	CStateCameraV::LimitPos(pCamera);

	MyLib::Vector3 posVDest = pCamera->GetPositionVDest();

	CListManager<CLimitArea> areaList = CLimitArea::GetListObj();
	CLimitArea* pArea = nullptr;

	float radius = 50.0f;

	while (areaList.ListLoop(&pArea))
	{
		CLimitArea::sLimitEreaInfo info = pArea->GetLimitEreaInfo();

		// 大人の壁を適用
		//if (posVDest.x + radius >= info.fMaxX) { posVDest.x = info.fMaxX - radius; }
		//if (posVDest.x - radius <= info.fMinX) { posVDest.x = info.fMinX + radius; }
		if (posVDest.z + radius >= info.fMaxZ) { posVDest.z = info.fMaxZ - radius; }
		//if (posVDest.z - radius <= info.fMinZ) { posVDest.z = info.fMinZ + radius; }
	}
	pCamera->SetPositionVDest(posVDest);

}

//==========================================================================
// ロックオン中の注視点設定
//==========================================================================
void CStateCameraR_RockOn::SetCameraR(CCamera* pCamera)
{
	MyLib::Vector3 targetpos = pCamera->GetTargetPosition();
	MyLib::Vector3 rot = pCamera->GetRotation();

	MyLib::Vector3 posdest;
	posdest.x = (targetpos.x);
	posdest.z = (targetpos.z);
	pCamera->SetPositionRDest(posdest);
}

//==========================================================================
// 祈り中の注視点設定
//==========================================================================
void CStateCameraR_Prayer::SetCameraR(CCamera* pCamera)
{
	MyLib::Vector3 targetpos = pCamera->GetTargetPosition();
	pCamera->SetPositionRDest(targetpos);
}



//==========================================================================
// 基底の操作処理
//==========================================================================
void CCameraControlState::MoveCamera(CCamera* pCamera)
{
	// ゲームパッド情報取得
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	MyLib::Vector3 moverot = pCamera->GetMoveRot();
	MyLib::Vector3 rot = pCamera->GetRotation();

	if (pCamera->IsRotationY())
	{
		moverot.y += pInputGamepad->GetStickMoveR(0).x * ROT_MOVE_STICK_Y;
	}

	if (rot.z > MIN_STICKROT &&
		(pCamera->IsRotationZ() || pInputGamepad->GetStickMoveR(0).y < 0.0f))
	{
		moverot.z += pInputGamepad->GetStickMoveR(0).y * ROT_MOVE_STICK_Z;
	}
	else if (rot.z <= MIN_STICKROT && pInputGamepad->GetStickMoveR(0).y > 0.0f)
	{
		moverot.z += pInputGamepad->GetStickMoveR(0).y * ROT_MOVE_STICK_Z;
	}

	// 移動する
	rot += moverot;

	UtilFunc::Correction::InertiaCorrection(moverot.y, 0.0f, 0.25f);
	UtilFunc::Correction::InertiaCorrection(moverot.z, 0.0f, 0.25f);

	// 回転移動量設定
	pCamera->SetRotation(rot);
	pCamera->SetMoveRot(moverot);
}

//==========================================================================
// ロックオンの操作処理
//==========================================================================
void CCameraControlState_RockOn::MoveCamera(CCamera* pCamera)
{
	// ゲームパッド情報取得
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	MyLib::Vector3 moverot = pCamera->GetMoveRot();
	MyLib::Vector3 rot = pCamera->GetRotation();
	MyLib::Vector3 rotdest = pCamera->GetDestRotation();

	// 目標の向き
	float fRotDiff = rotdest.z - rot.z;
	UtilFunc::Transformation::RotNormalize(fRotDiff);
	rot.z += fRotDiff * 0.08f;
	UtilFunc::Transformation::RotNormalize(rot.z);
	pCamera->SetRotation(rot);

	// ロックオン状態別処理
	(pCamera->*(CCamera::m_RockOnStateFunc[pCamera->GetRockOnState()]))();
}

//==========================================================================
// 祈り準備の操作処理
//==========================================================================
void CCameraControlState_BeforePrayer::MoveCamera(CCamera* pCamera)
{
	// 情報取得
	MyLib::Vector3 rot = pCamera->GetRotation();

	// 目標の向き
	MyLib::Vector3 rotDiff = DEFAULT_GAMEROT - rot;
	UtilFunc::Transformation::RotNormalize(rotDiff);

	rot += rotDiff * 0.08f;
	UtilFunc::Transformation::RotNormalize(rot);

	// 向き設定
	pCamera->SetRotation(rot);
}

//==========================================================================
// 祈りの操作処理
//==========================================================================
void CCameraControlState_Prayer::MoveCamera(CCamera* pCamera)
{
	// 情報取得
	MyLib::Vector3 rot = pCamera->GetRotation();
	MyLib::Vector3 targetpos = pCamera->GetTargetPosition();

	// 目標値
	//const MyLib::Vector3& rotdest = pCamera->GetDestRotation();
	const MyLib::Vector3& rotdest = ROTATION_PRAYER;

	// 目標の向き
	MyLib::Vector3 rotDiff = rotdest - rot;
	UtilFunc::Transformation::RotNormalize(rotDiff);

	rot += rotDiff * 0.08f;
	UtilFunc::Transformation::RotNormalize(rot);

	// 長さ設定
	pCamera->SetLenDest(170.0f, 10, 50.0f, 0.1f);

	// 向き設定
	pCamera->SetRotation(rot);
}