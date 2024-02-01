//=============================================================================
// 
//  プレイヤー処理 [player.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "player.h"
#include "game.h"
#include "camera.h"
#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "input.h"
#include "enemy.h"
#include "calculation.h"
#include "score.h"
#include "texture.h"
#include "Xload.h"
#include "model.h"
#include "hp_gauge.h"
#include "hp_gauge_player.h"
#include "stamina_gauge_player.h"
#include "elevation.h"
#include "shadow.h"
#include "particle.h"
#include "3D_Effect.h"
#include "ballast.h"
#include "impactwave.h"
#include "sound.h"
#include "stage.h"
#include "objectX.h"
#include "instantfade.h"
#include "fade.h"
#include "listmanager.h"
#include "collisionobject.h"
#include "limitarea.h"
#include "particle.h"
#include "MyEffekseer.h"
#include "skillpoint.h"
#include "damagepoint.h"
#include "deadmanager.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* CHARAFILE = "data\\TEXT\\character\\player\\tyuuni\\setup_player.txt";	// キャラクターファイル
	const float JUMP = 20.0f * 1.5f;	// ジャンプ力初期値
	const int INVINCIBLE_INT = 2;		// 無敵の間隔
	const int INVINCIBLE_TIME = 90;		// 無敵の時間
	const int DEADTIME = 120;			// 死亡時の時間
	const int FADEOUTTIME = 60;			// フェードアウトの時間
	const float MULTIPLIY_DASH = 1.875f;	// ダッシュの倍率
	const float RADIUS_STAGE = 2300.0f;	// ステージの半径
	const float TIME_DASHATTACK = 0.3f;		// ダッシュ攻撃に必要な時間
	const int DEFAULT_STAMINA = 200;	// スタミナのデフォルト値
	const float SUBVALUE_DASH = 0.3f;		// ダッシュの減算量
	const float SUBVALUE_AVOID = 30.0f;		// 回避の減算量
	const float SUBVALUE_COUNTER = 40.0f;	// カウンターの減算量
	const int DEFAULT_RESPAWN_PERCENT = 80;	// 復活確率のデフォルト値
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CPlayer::STATE_FUNC CPlayer::m_StateFunc[] =
{
	&CPlayer::StateNone,		// なし
	&CPlayer::StateInvincible,	// 無敵
	&CPlayer::StateDamage,		// ダメージ
	&CPlayer::StateKnockBack,	// ノックバック
	&CPlayer::StateDead,		// 死亡
	&CPlayer::StateDeadWait,	// 死亡待機
	&CPlayer::StateFadeOut,		// フェードアウト
	&CPlayer::StateRespawn,		// リスポーン
	&CPlayer::StateCounter,		// カウンター中
	&CPlayer::StateAvoid,		// 回避
};

//==========================================================================
// 静的メンバ変数
//==========================================================================
CListManager<CPlayer> CPlayer::m_List = {};	// リスト

//==========================================================================
// コンストラクタ
//==========================================================================
CPlayer::CPlayer(int nPriority) : CObjectChara(nPriority)
{
	// 値のクリア
	// 共有変数
	m_bJump = false;				// ジャンプ中かどうか
	m_bLandOld = false;				// 過去の着地情報
	m_bHitStage = false;			// ステージの当たり判定
	m_bLandField = false;			// フィールドの着地判定
	m_bHitWall = false;				// 壁の当たり判定
	m_nCntWalk = 0;					// 歩行カウンター
	m_state = STATE_NONE;			// 状態

	SMotionFrag initFrag = {};
	m_sMotionFrag = initFrag;		// モーションのフラグ

	// プライベート変数
	m_Oldstate = STATE_NONE;						// 前回の状態
	m_mMatcol = mylib_const::DEFAULT_COLOR;			// マテリアルの色
	m_posKnokBack = mylib_const::DEFAULT_VECTOR3;	// ノックバックの位置
	m_KnokBackMove = mylib_const::DEFAULT_VECTOR3;	// ノックバックの移動量
	m_nCntState = 0;								// 状態遷移カウンター
	m_nComboStage = 0;								// コンボの段階
	m_nIdxRockOn = 0;								// ロックオン対象のインデックス番号
	m_bLockOnAtStart = false;						// カウンター開始時にロックオンしていたか
	m_bAttacking = false;							// 攻撃中
	m_bCounterAccepting = false;					// カウンター受付中
	m_bDash = false;								// ダッシュ判定
	m_fDashTime = 0.0f;								// ダッシュ時間
	m_nRespawnPercent = 0;							// リスポーン確率
	m_bTouchBeacon = false;							// ビーコンに触れてる判定
	m_nMyPlayerIdx = 0;								// プレイヤーインデックス番号
	m_pShadow = NULL;								// 影の情報
	m_pSkillPoint = nullptr;						// スキルポイントのオブジェクト
	m_pHPGauge = nullptr;							// HPゲージのポインタ
	m_pStaminaGauge = nullptr;						// スタミナゲージのポインタ
	m_pEndCounterSetting = nullptr;					// カウンター終了時の設定
	m_pWeaponHandle = nullptr;		// エフェクトの武器ハンドル
}

//==========================================================================
// デストラクタ
//==========================================================================
CPlayer::~CPlayer()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CPlayer *CPlayer::Create(int nIdx)
{
	// 生成用のオブジェクト
	CPlayer *pPlayer = NULL;

	if (pPlayer == NULL)
	{// NULLだったら

		// メモリの確保
		pPlayer = DEBUG_NEW CPlayer;

		if (pPlayer != NULL)
		{// メモリの確保が出来ていたら

			// プレイヤーインデックス番号
			pPlayer->m_nMyPlayerIdx = nIdx;

			// 初期化処理
			pPlayer->Init();
		}

		return pPlayer;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CPlayer::Init(void)
{
	// 種類の設定
	SetType(TYPE_PLAYER);

	m_state = STATE_NONE;	// 状態
	m_nCntState = 0;		// 状態遷移カウンター
	m_bLandOld = true;		// 前回の着地状態
	m_nRespawnPercent = DEFAULT_RESPAWN_PERCENT;	// リスポーン確率

	// キャラ作成
	HRESULT hr = SetCharacter(CHARAFILE);
	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// 影の生成
	//m_pShadow = CShadow::Create(pos, 50.0f);

	// 割り当て
	m_List.Regist(this);

	// スキルポイント生成
	m_pSkillPoint = CSkillPoint::Create();

	// HPゲージ生成
	m_pHPGauge = CHP_GaugePlayer::Create({200.0f, 630.0f, 0.0f}, GetLifeOrigin());

	// スタミナゲージ生成
	m_pStaminaGauge = CStaminaGauge_Player::Create(MyLib::Vector3(200.0f, 680.0f, 0.0f), DEFAULT_STAMINA);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CPlayer::Uninit(void)
{
	// 影を消す
	if (m_pShadow != NULL)
	{
		//m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	if (m_pSkillPoint != nullptr)
	{
		m_pSkillPoint = nullptr;
	}

	// HPゲージ
	if (m_pHPGauge != nullptr)
	{
		m_pHPGauge = nullptr;
	}

	// 反撃終了時の設定
	if (m_pEndCounterSetting != nullptr)
	{
		delete m_pEndCounterSetting;
		m_pEndCounterSetting = nullptr;
	}

	// 終了処理
	CObjectChara::Uninit();

	// 削除
	m_List.Delete(this);
}

//==========================================================================
// 終了処理
//==========================================================================
void CPlayer::Kill(void)
{

	my_particle::Create(GetPosition(), my_particle::TYPE_ENEMY_FADE);

	if (m_pSkillPoint != nullptr)
	{
		m_pSkillPoint->Uninit();
		m_pSkillPoint = nullptr;
	}

	// HPゲージ
	if (m_pHPGauge != nullptr)
	{
		m_pHPGauge->Kill();
		m_pHPGauge = nullptr;
	}

	// スタミナゲージ
	if (m_pStaminaGauge != nullptr)
	{
		m_pStaminaGauge->Kill();
		m_pStaminaGauge = nullptr;
	}

	// 影を消す
	if (m_pShadow != NULL)
	{
		m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	// 反撃終了時の設定
	if (m_pEndCounterSetting != nullptr)
	{
		delete m_pEndCounterSetting;
		m_pEndCounterSetting = nullptr;
	}

	// ロックオン設定
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	if (pCamera != nullptr)
	{
		pCamera->SetRockOn(0.0f, false);
	}

	// リストループ
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;
	while (enemyList.ListLoop(&pEnemy))
	{
		pEnemy->SetEnableRockOn(false);
	}

}

//==========================================================================
// 更新処理
//==========================================================================
void CPlayer::Update(void)
{
	if (IsDeath() == true)
	{
		return;
	}

	// キーボード情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	if (pInputKeyboard->GetTrigger(DIK_F5) == true)
	{// F5でリセット
		SetPosition(MyLib::Vector3(0.0f, 0.0f, -1000.0f));
		SetMove(MyLib::Vector3(0.0f, 0.0f, 0.0f));
	}

	// エディット中は抜ける
	if (CGame::GetInstance()->GetElevation()->IsEdit())
	{
		return;
	}

	// エディット中は抜ける
	if (CGame::GetInstance()->GetEditType() != CGame::GetInstance()->EDITTYPE_OFF)
	{
		return;
	}

	// 過去の位置保存
	SetOldPosition(GetPosition());

	// フラグリセット
	ResetFrag();

	// 親の更新処理
	CObjectChara::Update();

	// 操作
	Controll();

	// モーションの設定処理
	MotionSet();

	// モーション別の状態設定
	MotionBySetState();

	// 状態更新
	(this->*(m_StateFunc[m_state]))();

	// 位置取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 posCenter = GetCenterPosition();

	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// 向き取得
	MyLib::Vector3 rot = GetRotation();

	// カメラの情報取得
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->SetTargetPosition(pos);
	pCamera->SetTargetRotation(rot);

	// 影の位置更新
	if (m_pShadow != NULL)
	{
		m_pShadow->SetPosition(MyLib::Vector3(pos.x, m_pShadow->GetPosition().y, pos.z));
	}

	if (m_pHPGauge != nullptr)
	{
		m_pHPGauge->SetLife(GetLife());
	}


	int nCntEffect = 0;
	int nNumEffect = GetEffectParentNum();
	for (int i = 0; i < mylib_const::MAX_OBJ; i++)
	{
		CEffect3D* pEffect = GetEffectParent(i);
		if (pEffect == NULL)
		{// NULLだったら
			continue;
		}

		// エフェクトの位置更新
		pEffect->UpdatePosition(GetRotation());
		nCntEffect++;
		if (nNumEffect <= nCntEffect)
		{
			break;
		}
	}

	// 位置の制限
	LimitPos();

#if 1
	// デバッグ表示
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[プレイヤーの操作]------------------\n"
		"位置：【X：%f, Y：%f, Z：%f】 【W / A / S / D】\n"
		"向き：【X：%f, Y：%f, Z：%f】 【Z / C】\n"
		"移動量：【X：%f, Y：%f, Z：%f】\n"
		"体力：【%d】\n"
		"コンボステージ：【%d】\n"
		"ダッシュ時間：【%f】\n"
		, pos.x, pos.y, pos.z, rot.x, rot.y, rot.y, move.x, move.y, move.z, GetLife(), m_nComboStage, m_fDashTime);
#endif

}

//==========================================================================
// 操作処理
//==========================================================================
void CPlayer::Controll(void)
{

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// カメラの情報取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// カメラの向き取得
	MyLib::Vector3 Camerarot = pCamera->GetRotation();

	// 位置取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 newPosition = GetPosition();
	MyLib::Vector3 sakiPos = GetPosition();

	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// 向き取得
	MyLib::Vector3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// 現在の種類取得
	CMotion* pMotion = GetMotion();
	int nMotionType = pMotion->GetType();

	// 移動量取得
	float fMove = GetVelocity();

	// 経過時間取得
	float fCurrentTime = CManager::GetInstance()->GetDeltaTime();

	if (CGame::GetInstance()->GetGameManager()->IsControll())
	{// 行動できるとき

		// ダッシュ判定
		if (pInputGamepad->GetPress(CInputGamepad::BUTTON_LB, m_nMyPlayerIdx) &&
			pInputGamepad->IsTipStick())
		{// 左スティックが倒れてる場合
			m_bDash = true;
		}
		else
		{
			m_bDash = false;
		}

		if (m_bDash)
		{
			// ダッシュ倍率掛ける
			fMove *= MULTIPLIY_DASH;
		}

		if ((pMotion->IsGetMove(nMotionType) == 1 || pMotion->IsGetCancelable()) &&
			!m_sMotionFrag.bATK &&
			m_state != STATE_DMG &&
			m_state != STATE_KNOCKBACK &&
			m_state != STATE_DEAD &&
			m_state != STATE_FADEOUT)
		{// 移動可能モーションの時

			if (pInputKeyboard->GetPress(DIK_A) == true)
			{//←キーが押された,左移動

				// 移動中にする
				m_sMotionFrag.bMove = true;

				if (pInputKeyboard->GetPress(DIK_W) == true)
				{//A+W,左上移動

					move.x += sinf(-D3DX_PI * 0.25f + Camerarot.y) * fMove;
					move.z += cosf(-D3DX_PI * 0.25f + Camerarot.y) * fMove;
					fRotDest = D3DX_PI * 0.75f + Camerarot.y;
				}
				else if (pInputKeyboard->GetPress(DIK_S) == true)
				{//A+S,左下移動

					move.x += sinf(-D3DX_PI * 0.75f + Camerarot.y) * fMove;
					move.z += cosf(-D3DX_PI * 0.75f + Camerarot.y) * fMove;
					fRotDest = D3DX_PI * 0.25f + Camerarot.y;
				}
				else
				{//A,左移動

					move.x += sinf(-D3DX_PI * 0.5f + Camerarot.y) * fMove;
					move.z += cosf(-D3DX_PI * 0.5f + Camerarot.y) * fMove;
					fRotDest = D3DX_PI * 0.5f + Camerarot.y;
				}
			}
			else if (pInputKeyboard->GetPress(DIK_D) == true)
			{//Dキーが押された,右移動

				// 移動中にする
				m_sMotionFrag.bMove = true;

				if (pInputKeyboard->GetPress(DIK_W) == true)
				{//D+W,右上移動

					move.x += sinf(D3DX_PI * 0.25f + Camerarot.y) * fMove;
					move.z += cosf(D3DX_PI * 0.25f + Camerarot.y) * fMove;
					fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
				}
				else if (pInputKeyboard->GetPress(DIK_S) == true)
				{//D+S,右下移動

					move.x += sinf(D3DX_PI * 0.75f + Camerarot.y) * fMove;
					move.z += cosf(D3DX_PI * 0.75f + Camerarot.y) * fMove;
					fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
				}
				else
				{//D,右移動

					move.x += sinf(D3DX_PI * 0.5f + Camerarot.y) * fMove;
					move.z += cosf(D3DX_PI * 0.5f + Camerarot.y) * fMove;
					fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
				}
			}
			else if (pInputKeyboard->GetPress(DIK_W) == true)
			{//Wが押された、上移動

				// 移動中にする
				m_sMotionFrag.bMove = true;
				move.x += sinf(D3DX_PI * 0.0f + Camerarot.y) * fMove;
				move.z += cosf(D3DX_PI * 0.0f + Camerarot.y) * fMove;
				fRotDest = D3DX_PI * 1.0f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true)
			{//Sが押された、下移動

				// 移動中にする
				m_sMotionFrag.bMove = true;
				move.x += sinf(D3DX_PI * 1.0f + Camerarot.y) * fMove;
				move.z += cosf(D3DX_PI * 1.0f + Camerarot.y) * fMove;
				fRotDest = D3DX_PI * 0.0f + Camerarot.y;
			}
			else
			{
				// 移動中かどうか
				m_sMotionFrag.bMove = false;
			}

			if (pInputGamepad->IsTipStick())
			{// 左スティックが倒れてる場合

				// 移動中にする
				m_sMotionFrag.bMove = true;

				// スティックの向き取得
				float stickrot = pInputGamepad->GetStickRotL(m_nMyPlayerIdx);
				UtilFunc::Transformation::RotNormalize(stickrot);

				// 移動量と向き設定
				move.x += sinf(stickrot + Camerarot.y) * fMove;
				move.z += cosf(stickrot + Camerarot.y) * fMove;
				fRotDest = D3DX_PI + stickrot + Camerarot.y;
			}

			if (m_sMotionFrag.bMove &&
				!m_bJump)
			{// キャンセル可能 && 移動中

				pMotion->ToggleFinish(true);
			}

			if (m_bJump == false &&
				(pInputKeyboard->GetTrigger(DIK_SPACE) == true ||
				pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, m_nMyPlayerIdx)) &&
				!m_bTouchBeacon)
			{// ジャンプ

				m_bJump = true;
				m_sMotionFrag.bJump = true;
				move.y += 17.0f;

				// サウンド再生
				CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_JUMP);
			}
		}
		else if (pMotion->IsGetMove(nMotionType) == 0 &&
			m_state != STATE_DEAD &&
			m_state != STATE_FADEOUT)
		{
			if (pInputKeyboard->GetPress(DIK_A) == true)
			{//←キーが押された,左移動

				if (pInputKeyboard->GetPress(DIK_W) == true)
				{//A+W,左上移動
					fRotDest = D3DX_PI * 0.75f + Camerarot.y;
				}
				else if (pInputKeyboard->GetPress(DIK_S) == true)
				{//A+S,左下移動
					fRotDest = D3DX_PI * 0.25f + Camerarot.y;
				}
				else
				{//A,左移動
					fRotDest = D3DX_PI * 0.5f + Camerarot.y;
				}
			}
			else if (pInputKeyboard->GetPress(DIK_D) == true)
			{//Dキーが押された,右移動

				if (pInputKeyboard->GetPress(DIK_W) == true)
				{//D+W,右上移動
					fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
				}
				else if (pInputKeyboard->GetPress(DIK_S) == true)
				{//D+S,右下移動
					fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
				}
				else
				{//D,右移動
					fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
				}
			}
			else if (pInputKeyboard->GetPress(DIK_W) == true)
			{//Wが押された、上移動
				fRotDest = D3DX_PI * 1.0f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true)
			{//Sが押された、下移動
				fRotDest = D3DX_PI * 0.0f + Camerarot.y;
			}

			//if (pInputGamepad->IsTipStick())
			//{// 左スティックが倒れてる場合

			//	fRotDest = D3DX_PI + pInputGamepad->GetStickRotL(m_nMyPlayerIdx) + Camerarot.y;
			//}
		}

		// 攻撃
		if ((pMotion->IsGetCombiable() || pMotion->IsGetCancelable()) &&
			!m_bJump &&
			!pInputGamepad->GetPress(CInputGamepad::BUTTON_RB, m_nMyPlayerIdx) && 
			pInputGamepad->GetTrigger(CInputGamepad::BUTTON_X, m_nMyPlayerIdx))
		{
			//pMotion->ToggleFinish(true);
			//m_sMotionFrag.bATK = true;		// 攻撃判定ON

			if (pInputGamepad->IsTipStick())
			{// 左スティックが倒れてる場合
				fRotDest = D3DX_PI + pInputGamepad->GetStickRotL(m_nMyPlayerIdx) + Camerarot.y;
			}

			if (m_fDashTime >= TIME_DASHATTACK && m_nComboStage == 0)
			{// ダッシュ中の初撃は2からスタート
				m_nComboStage++;
				pMotion->Set(MOTION_DASHATK, true);
			}
			else
			{
				// コンボ段階分考慮
				int nSetType = MOTION_ATK + m_nComboStage;
				pMotion->Set(nSetType, true);
			}

			// コンボの段階加算
			m_nComboStage++;
			if (m_nComboStage > MOTION_ATK3 - MOTION_ATK)
			{
				m_nComboStage = 0;
			}
		}

		// カウンター
		if ((pMotion->IsGetCombiable() || pMotion->IsGetCancelable()) &&
			!m_bJump &&
			pInputGamepad->GetPress(CInputGamepad::BUTTON_RB, m_nMyPlayerIdx) &&
			pInputGamepad->GetTrigger(CInputGamepad::BUTTON_X, m_nMyPlayerIdx))
		{
			pMotion->Set(MOTION_DEF);
			pMotion->Set(MOTION_COUNTER_ACCEPT);
			m_sMotionFrag.bCounter = true;		// 攻撃判定ON

			if (pInputGamepad->IsTipStick())
			{// 左スティックが倒れてる場合

				fRotDest = D3DX_PI + pInputGamepad->GetStickRotL(m_nMyPlayerIdx) + Camerarot.y;
			}

			// スタミナ減算
			if (m_pStaminaGauge != nullptr)
			{
				m_pStaminaGauge->SubValue(SUBVALUE_COUNTER);
			}
		}


		// デバッグ表示
		CManager::GetInstance()->GetDebugProc()->Print(
			"------------------[プレイヤーの操作]------------------\n"
			"コンボステージ：【%d】\n"
			, m_nComboStage);

		// 回避
		if (!m_bJump &&
			(pMotion->IsGetCombiable() || pMotion->IsGetCancelable()) &&
			pInputGamepad->GetTrigger(CInputGamepad::BUTTON_B, m_nMyPlayerIdx))
		{
			pMotion->Set(MOTION_DEF);
			pMotion->Set(MOTION_AVOID);

			if (pInputGamepad->IsTipStick())
			{// 左スティックが倒れてる場合

				fRotDest = D3DX_PI + pInputGamepad->GetStickRotL(m_nMyPlayerIdx) + Camerarot.y;
			}
			m_state = STATE_AVOID;

			// スタミナ減算
			if (m_pStaminaGauge != nullptr)
			{
				m_pStaminaGauge->SubValue(SUBVALUE_AVOID);
			}
		}
	}

	// 移動量加算
	newPosition.x += move.x;
	newPosition.z += move.z;

	sakiPos.x = newPosition.x + sinf(D3DX_PI + rot.y) * GetRadius();
	sakiPos.z = newPosition.z + cosf(D3DX_PI + rot.y) * GetRadius();

	// 角度の正規化
	UtilFunc::Transformation::RotNormalize(fRotDest);
	SetRotDest(fRotDest);

	// 現在と目標の差分を求める
	float fRotDiff = fRotDest - rot.y;

	// 角度の正規化
	UtilFunc::Transformation::RotNormalize(fRotDiff);

	// 角度の補正をする
	rot.y += fRotDiff * 0.25f;
	UtilFunc::Transformation::RotNormalize(rot.y);

	// 向き設定
	SetRotation(rot);


	// 重力処理
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_FADEOUT && m_state != STATE_DEADWAIT)
	{
		move.y -= mylib_const::GRAVITY;

		// 位置更新
		newPosition.y += move.y;
		sakiPos.y = newPosition.y;
	}

	//**********************************
	// 当たり判定
	//**********************************
	bool bLandStage = Collision(sakiPos, move);

	bool bMove = false;
	if (m_bLandOld == false && bLandStage == true)
	{// 前回は着地していなくて、今回は着地している場合

		bMove = false;
	}

	if (m_bLandOld == true && bLandStage == true)
	{// 前回も今回も着地している場合
		bMove = true;
	}

	if (m_bHitWall == false && 
		(bLandStage == false || bMove == true || m_bLandField == true || m_bJump == true || m_sMotionFrag.bKnockBack == true || m_sMotionFrag.bDead == true))
	{
		pos.x = newPosition.x;
		pos.z = newPosition.z;
		pos.y = sakiPos.y;
		Collision(pos, move);

		// 前回は乗ってたことにする
		m_bLandOld = true;
	}
	else
	{
		MyLib::Vector3 posOld = GetOldPosition();
		pos.x = posOld.x;
		pos.z = posOld.z;
		pos = posOld;
		move.x = 0.0f;
		pos.y -= mylib_const::GRAVITY * 7.0f;

		if (m_bJump == false)
		{
			m_bJump = true;
		}

		Collision(pos, move);
	}

	// 慣性補正
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_FADEOUT)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// ロックオン対象切り替え
	if (CManager::GetInstance()->GetCamera()->IsRockOn())
	{
		SwitchRockOnTarget();
	}

	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_RSTICKPUSH, m_nMyPlayerIdx))
	{
		if (CManager::GetInstance()->GetCamera()->IsRockOn())
		{// ロックオン解除

			// ロックオン設定
			CManager::GetInstance()->GetCamera()->SetRockOn(0.0f, false);

			// リストループ
			CListManager<CEnemy> enemyList = CEnemy::GetListObj();
			CEnemy* pEnemy = nullptr;
			while (enemyList.ListLoop(&pEnemy))
			{
				pEnemy->SetEnableRockOn(false);
			}
		}
		else
		{
			// ロックオン処理
			RockOn();
		}
	}

	// リストループ
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;
	while (enemyList.ListLoop(&pEnemy))
	{
		if (CGame::GetInstance()->GetRockOnDistance() <= UtilFunc::Calculation::GetPosLength3D(pos, pEnemy->GetPosition()))
		{
			if (pEnemy->IsRockOnAccept())
			{
				// ロックオン設定
				CManager::GetInstance()->GetCamera()->SetRockOn(0.0f, false);
			}

			pEnemy->SetEnableRockOn(false);
		}
	}


	if (pInputKeyboard->GetTrigger(DIK_LEFT) == true)
	{
		CCollisionObject::Create(GetPosition(), mylib_const::DEFAULT_VECTOR3, 100000.0f, 3, 10000, CCollisionObject::TAG_PLAYER);
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{
		m_sMotionFrag.bATK = true;		// 攻撃判定OFF

		MyLib::Vector3 weponpos = pos;

		CMyEffekseer::GetInstance()->SetEffect(
			&m_pWeaponHandle,
			"data/Effekseer/debugline_green.efkefc",
			weponpos, rot, 0.0f, 40.0f, true);

		// デバッグ表示
		CManager::GetInstance()->GetDebugProc()->Print(
			"わああああああああああああああああああああああああああああああああああああああああああああ\n");
	}

	if (pInputKeyboard->GetTrigger(DIK_SPACE) == true)
	{
		MyLib::Vector3 weponpos = pos;
		weponpos.y += 150.0f;

		*m_pWeaponHandle = CMyEffekseer::GetInstance()->SetEffect(
			"data/Effekseer/MyLine.efkefc",
			weponpos, rot, 0.0f, 40.0f);
	}

	if (pInputKeyboard->GetRepeat(DIK_O, 4) == true)
	{
		MyLib::Vector3 weponpos = pos;
		weponpos.y += 150.0f;

		MyLib::Vector3 spawnpos = UtilFunc::Transformation::GetRandomPositionSphere(weponpos, 300.0f);

		CMyEffekseer::GetInstance()->SetEffect(
			"data/Effekseer/Laser01.efkefc",
			spawnpos, UtilFunc::Transformation::GetRandomVecSphere() * D3DX_PI, mylib_const::DEFAULT_VECTOR3, 10.0f);
	}
	if (pInputKeyboard->GetTrigger(DIK_L) == true)
	{
		MyLib::Vector3 weponpos = pos;
		weponpos.y += 150.0f;

		MyLib::Vector3 spawnpos = UtilFunc::Transformation::GetRandomPositionSphere(weponpos, 300.0f);

		CMyEffekseer::GetInstance()->SetEffect(
			"data/Effekseer/CounterParticle_01.efkefc",
			weponpos, MyLib::Vector3(1.57f, 1.57f, 1.57f), 0.0f, 50.0f);
	}

	static float fff = 1.0f;
	if (pInputKeyboard->GetTrigger(DIK_UP) == true)
	{
		fff += 0.1f;
		CManager::GetInstance()->GetSound()->SetFrequency(CSound::LABEL_BGM_GAME, fff);
	}
	if (pInputKeyboard->GetTrigger(DIK_DOWN) == true)
	{
		fff -= 0.1f;
		CManager::GetInstance()->GetSound()->SetFrequency(CSound::LABEL_BGM_GAME, fff);
	}

	if (m_pWeaponHandle != nullptr)
	{

		// 武器の位置
		MyLib::Vector3 weponpos = UtilFunc::Transformation::WorldMtxChangeToPosition(GetModel()[16]->GetWorldMtx());

		// 武器のマトリックス取得
		D3DXMATRIX weaponWorldMatrix = GetModel()[16]->GetWorldMtx();

		// 軌跡のマトリックス設定
		CMyEffekseer::GetInstance()->SetMatrix(*m_pWeaponHandle, weaponWorldMatrix);
		CMyEffekseer::GetInstance()->SetPosition(*m_pWeaponHandle, weponpos);
	}
}

//==========================================================================
// モーションの設定
//==========================================================================
void CPlayer::SetMotion(int motionIdx)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}
	pMotion->Set(motionIdx);
}

//==========================================================================
// モーションの設定
//==========================================================================
void CPlayer::MotionSet(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	if (pMotion->IsFinish() == true)
	{// 終了していたら

		// 現在の種類取得
		int nType = pMotion->GetType();
		int nOldType = pMotion->GetOldType();

		if (m_sMotionFrag.bMove == true && m_sMotionFrag.bKnockBack == false && m_bJump == false &&
			m_sMotionFrag.bATK == false && m_sMotionFrag.bATK == false)
		{// 移動していたら

			m_sMotionFrag.bMove = false;	// 移動判定OFF

			// 移動モーション
			if (m_bDash)
			{
				pMotion->Set(MOTION_DASH);
			}
			else
			{
				pMotion->Set(MOTION_WALK);
			}
		}
		else if (m_sMotionFrag.bJump == true && m_sMotionFrag.bATK == false && m_sMotionFrag.bKnockBack == false && m_sMotionFrag.bDead == false)
		{// ジャンプ中

			// ジャンプのフラグOFF
			m_sMotionFrag.bJump = false;

			// ジャンプモーション
			pMotion->Set(MOTION_JUMP);
		}
		else if (m_bJump == true && m_sMotionFrag.bJump == false && m_sMotionFrag.bATK == false && m_sMotionFrag.bKnockBack == false && m_sMotionFrag.bDead == false)
		{// ジャンプ中&&ジャンプモーションが終わってる時

			// 落下モーション
			pMotion->Set(MOTION_FALL);
		}
		else if (m_sMotionFrag.bKnockBack == true)
		{// やられ中だったら

			// やられモーション
			pMotion->Set(MOTION_KNOCKBACK);
		}
		//else if (m_sMotionFrag.bATK == true)
		//{// 攻撃

		//	m_sMotionFrag.bATK = false;		// 攻撃判定OFF

		//	int nSetType = MOTION_ATK + m_nComboStage;
		//	pMotion->Set(nSetType, true);

		//	if (m_nComboStage >= MOTION_ATK3 - MOTION_ATK)
		//	{
		//		m_nComboStage = 0;
		//	}
		//}
		else
		{
			// ニュートラルモーション
			pMotion->Set(MOTION_DEF);
		}
	}
}

//==========================================================================
// モーション別の状態設定
//==========================================================================
void CPlayer::MotionBySetState(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}
	int nType = pMotion->GetType();

	switch (nType)
	{
	case MOTION_DASH:

		// ダッシュ時間加算
		m_fDashTime += CManager::GetInstance()->GetDeltaTime();

		if (m_pStaminaGauge != nullptr)
		{
			m_pStaminaGauge->SubValue(SUBVALUE_DASH);
		}
		break;

	default:
		m_fDashTime = 0.0f;
		break;
	}

	switch (nType)
	{
	case MOTION_ATK:
	case MOTION_ATK2:
	case MOTION_ATK3:
	case MOTION_DASHATK:
		m_bAttacking = true;
		break;

	case MOTION_COUNTER_TURN:
	case MOTION_COUNTER_ATTACK:
		// カウンター状態に設定
		m_state = STATE_COUNTER;
		break;

	default:
		m_bAttacking = false;	// 攻撃中判定
		m_nComboStage = 0;		// コンボの段階
		break;
	}
}

//==========================================================================
// フラグリセット
//==========================================================================
void CPlayer::ResetFrag(void)
{
	m_bCounterAccepting = false;	// カウンター受付中
}

//==========================================================================
// ロックオン
//==========================================================================
void CPlayer::RockOn(void)
{
	// カメラ取得
	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	// 敵のリスト取得
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	float fNearLen = CGame::GetInstance()->GetRockOnDistance();
	int nMaxIdx = 0;
	MyLib::Vector3 targetpos(0.0f);

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// リストループ
	int i = 0;
	MyLib::Vector3 enemypos(0.0f);
	while (enemyList.ListLoop(&pEnemy))
	{
		// 敵の位置取得
		enemypos = pEnemy->GetPosition();

		if (pCamera->IsOnScreen(enemypos))
		{
			float len = UtilFunc::Calculation::GetPosLength3D(pos, enemypos);
			if (fNearLen > len)
			{
				fNearLen = len;
				nMaxIdx = i;
			}
		}

		// インデックス加算
		i++;
	}

	if (fNearLen < CGame::GetInstance()->GetRockOnDistance())
	{// ロックオン距離内なら

		// ロックオン設定
		pCamera->SetRockOn(enemyList.GetData(nMaxIdx)->GetPosition(), true);
		enemyList.GetData(nMaxIdx)->SetEnableRockOn(true);

		// インデックス番号設定
		m_nIdxRockOn = nMaxIdx;
	}
}

//==========================================================================
// ロックオン対象切り替え
//==========================================================================
void CPlayer::SwitchRockOnTarget(void)
{
	// カメラ取得
	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// ゲームパッド情報取得
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// 敵のリスト取得
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	bool bSwitch = true;
	if (pInputGamepad->GetRStickTrigger(CInputGamepad::STICK_X))
	{// 左右どちらかに切り替え

		bool bSwitch = true;

		// リストループ
		int i = 0, nMaxIdx = m_nIdxRockOn;
		float fNearLen = CGame::GetInstance()->GetRockOnDistance();
		pEnemy = nullptr;
		MyLib::Vector3 enemypos(0.0f);
		while (enemyList.ListLoop(&pEnemy))
		{
			// 敵の位置取得
			enemypos = pEnemy->GetPosition();

			if (pCamera->IsOnScreen(enemypos))
			{
				float len = UtilFunc::Calculation::GetPosLength3D(pos, enemypos);
				if (fNearLen > len &&
					m_nIdxRockOn != i)
				{
					fNearLen = len;
					nMaxIdx = i;
				}
			}

			// インデックス加算
			i++;
		}

		// 今までロックオンしてた対象リセット
		enemyList.GetData(m_nIdxRockOn)->SetEnableRockOn(false);

		// ロックオン設定
		CEnemy* pSetEnemy = enemyList.GetData(nMaxIdx);
		CManager::GetInstance()->GetCamera()->SetRockOn(pSetEnemy->GetPosition(), true);
		pSetEnemy->SetEnableRockOn(true);

		// インデックス番号設定
		m_nIdxRockOn = nMaxIdx;
	}
}

//==========================================================================
// 攻撃時処理
//==========================================================================
void CPlayer::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	int nType = pMotion->GetType();

	// 武器の位置
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), ATKInfo);

	switch (nType)
	{
	case MOTION_ATK:	// 雪玉を拾う
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_SNOWGET);
		break;

	case MOTION_WALK:
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_WALK);
		break;

	case MOTION_COUNTER_ATTACK:

		if (nCntATK == 0)
		{
			CMyEffekseer::GetInstance()->SetEffect(
				"data/Effekseer/CounterParticle_01.efkefc",
				weponpos, 0.0f, 0.0f, 50.0f);
		}
		else if (nCntATK != 0)
		{
			CManager::GetInstance()->GetCamera()->SetLenDest(200.0f, 3, 4.0f, 0.3f);

			// 敵へダウン状態
			CEnemy::GetListObj().GetData(m_nIdxRockOn)->Hit(10, CGameManager::ATTACK_COUNTER);
		}
		break;

	default:
		break;
	}
}

//==========================================================================
// 攻撃判定中処理
//==========================================================================
void CPlayer::AttackInDicision(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// モーション取得
	CMotion* pMotion = GetMotion();

	// 武器の位置
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), ATKInfo);

	CEffect3D* pEffect = NULL;

	switch (pMotion->GetType())
	{
	case MOTION_ATK:
		break;

	case MOTION_COUNTER_ACCEPT:
		m_bCounterAccepting = true;	// カウンター受付中
		break;
	}

	if (ATKInfo.fRangeSize == 0.0f)
	{
		return;
	}

#if _DEBUG
	CEffect3D::Create(
		weponpos,
		MyLib::Vector3(0.0f, 0.0f, 0.0f),
		D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),
		ATKInfo.fRangeSize, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
#endif

	// 敵のリスト取得
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	// リストループ
	while (enemyList.ListLoop(&pEnemy))
	{
		// 敵の位置取得
		MyLib::Vector3 TargetPos = pEnemy->GetPosition();

		// 判定サイズ取得
		float fTargetRadius = pEnemy->GetRadius();

		MyLib::HitResult hitresult = UtilFunc::Collision::SphereRange(weponpos, TargetPos, ATKInfo.fRangeSize, fTargetRadius);
		if (hitresult.ishit)
		{// 球の判定

			if (pEnemy->Hit(ATKInfo.nDamage) == true)
			{// 当たってたら

				// 位置
				MyLib::Vector3 pos = GetPosition();
				MyLib::Vector3 enemypos = pEnemy->GetPosition();

				// ターゲットと敵との向き
				float fRot = atan2f((enemypos.x - pos.x), (enemypos.z - pos.z));
				UtilFunc::Transformation::RotNormalize(fRot);

				pEnemy->SetMove(MyLib::Vector3(sinf(fRot) * 8.0f, 0.0f, cosf(fRot) * 8.0f));

				CMyEffekseer::GetInstance()->SetEffect(
					"data/Effekseer/HitParticle_Red_01.efkefc",
					hitresult.hitpos, 0.0f, 0.0f, 50.0f);

				enemypos.y += pEnemy->GetHeight() * 0.5f;
				enemypos += UtilFunc::Transformation::GetRandomPositionSphere(enemypos, fTargetRadius * 0.5f);
				CDamagePoint::Create(hitresult.hitpos, ATKInfo.nDamage);
			}
		}
	}
}

//==========================================================================
// 位置制限
//==========================================================================
void CPlayer::LimitPos(void)
{
	MyLib::Vector3 pos = GetPosition();

	float fLength = sqrtf(pos.x * pos.x + pos.z * pos.z);

	if (fLength > RADIUS_STAGE)
	{// 補正
		D3DXVec3Normalize(&pos, &pos);

		pos *= RADIUS_STAGE;

		SetPosition(pos);
	}
}

//==========================================================================
// 当たり判定
//==========================================================================
bool CPlayer::Collision(MyLib::Vector3 &pos, MyLib::Vector3 &move)
{
	// 向き取得
	MyLib::Vector3 rot = GetRotation();

	// 着地したかどうか
	bool bLand = false;
	float fHeight = 0.0f;
	m_bLandField = false;
	m_bHitWall = false;			// 壁の当たり判定

	// 高さ取得
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_FADEOUT)
	{
		fHeight = CManager::GetInstance()->GetScene()->GetElevation()->GetHeight(pos, &bLand);
	}
	else
	{
		fHeight = pos.y;
	}

	if (fHeight > pos.y)
	{// 地面の方が自分より高かったら

		// 地面の高さに補正
		pos.y = fHeight;
		m_bLandField = true;

		if (bLand == true)
		{// 着地してたら

			// ジャンプ使用可能にする
			m_bJump = false;
			move.y = 0.0f;
			m_bLandOld = true;
		}
	}


	// Xファイルとの判定
	CStage *pStage = CGame::GetInstance()->GetStage();
	if (pStage == NULL)
	{// NULLだったら
		return false;
	}

	bool bNowLand = false;

	// ステージに当たった判定
	m_bHitStage = false;
	for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
	{
		// オブジェクト取得
		CObjectX *pObjX = pStage->GetObj(nCntStage);

		if (pObjX == NULL)
		{// NULLだったら
			continue;
		}

		// 高さ取得
		bool bLand = false;
		fHeight = pObjX->GetHeight(pos, bLand);

		if (bLand == true && fHeight > pos.y)
		{// 地面の方が自分より高かったら

		 // 地面の高さに補正
			if (pos.y + 50.0f <= fHeight)
			{// 自分より壁が高すぎる
				m_bHitWall = true;
			}
			else
			{
				pos.y = fHeight;
			}

			m_bHitStage = true;
			m_bLandField = false;

			if (bLand == true)
			{// 着地してたら

				if ((m_sMotionFrag.bKnockBack || m_bJump == true) && GetPosition().y >= fHeight)
				{
					m_bLandOld = true;
				}

				if (m_bJump == true)
				{// ジャンプ中だったら

					// モーション取得
					CMotion* pMotion = GetMotion();
					pMotion->ToggleFinish(true);
				}

				// ジャンプ使用可能にする
				m_bJump = false;
				move.y = 0.0f;
				bNowLand = true;
				m_sMotionFrag.bJump = false;
			}
		}
	}

	// エリア制限情報取得
	CListManager<CLimitArea> limitareaList = CLimitArea::GetListObj();
	CLimitArea* pLimitArea = nullptr;

	while (limitareaList.ListLoop(&pLimitArea))
	{
		CLimitArea::sLimitEreaInfo info = pLimitArea->GetLimitEreaInfo();

		// 大人の壁を適用
		if (pos.x + GetRadius() >= info.fMaxX) { pos.x = info.fMaxX - GetRadius(); }
		if (pos.x - GetRadius() <= info.fMinX) { pos.x = info.fMinX + GetRadius(); }
		if (pos.z + GetRadius() >= info.fMaxZ) { pos.z = info.fMaxZ - GetRadius(); }
		if (pos.z - GetRadius() <= info.fMinZ) { pos.z = info.fMinZ + GetRadius(); }
	}

	// 向き設定
	SetRotation(rot);

	return bNowLand;
}

//==========================================================================
// ヒット処理
//==========================================================================
bool CPlayer::Hit(const int nValue, CGameManager::AttackType atkType)
{
	bool bHit = false;

	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	if (m_bCounterAccepting)
	{// カウンター受け付け中
		
		if (atkType == CGameManager::ATTACK_NORMAL)
		{
			// 受け流し
			GetMotion()->Set(MOTION_COUNTER_TURN);

			// 受け流しの設定
			m_pEndCounterSetting = DEBUG_NEW CEndTurn;
		}
		else
		{
			if (GetMotion()->GetType() != MOTION_COUNTER_ATTACK)
			{
				// 開始時のフラグコピー
				m_bLockOnAtStart = pCamera->IsRockOn();

				// 反撃
				GetMotion()->Set(MOTION_COUNTER_ATTACK);
				pCamera->SetRockOnState(CCamera::RockOnState::ROCKON_COUNTER);
				m_pSkillPoint->AddPoint();

				// 攻撃の設定
				m_pEndCounterSetting = DEBUG_NEW CEndAttack;
			}
		}
		return false;
	}
	
	// 共通のヒット処理
	bHit = ProcessHit(nValue);

	// 当たった判定を返す
	return bHit;
}

//==========================================================================
// ヒット処理
//==========================================================================
bool CPlayer::Hit(const int nValue, CEnemy* pEnemy, CGameManager::AttackType atkType)
{
	bool bHit = false;

	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	if (m_bCounterAccepting)
	{// カウンター受け付け中

		if (atkType == CGameManager::ATTACK_NORMAL)
		{
			// 受け流し
			GetMotion()->Set(MOTION_COUNTER_TURN);

			// 受け流しの設定
			m_pEndCounterSetting = DEBUG_NEW CEndTurn;
		}
		else
		{
			if (GetMotion()->GetType() != MOTION_COUNTER_ATTACK)
			{

				// 開始時のフラグコピー
				m_bLockOnAtStart = pCamera->IsRockOn();
				if (!pCamera->IsRockOn())
				{
					// ロックオン設定
					pCamera->SetRockOn(pEnemy->GetPosition(), true);
					pEnemy->SetEnableRockOn(true);

					// インデックス番号設定
					m_nIdxRockOn = CEnemy::GetListObj().FindIdx(pEnemy);
				}

				// 反撃
				GetMotion()->Set(MOTION_COUNTER_ATTACK);
				pCamera->SetRockOnState(CCamera::RockOnState::ROCKON_COUNTER);
				m_pSkillPoint->AddPoint();

				// 攻撃の設定
				m_pEndCounterSetting = DEBUG_NEW CEndAttack;
			}
		}
		return false;
	}

	// 共通のヒット処理
	bHit = ProcessHit(nValue);

	// 当たった判定を返す
	return bHit;
}

//==========================================================================
// 共通のヒット処理
//==========================================================================
bool CPlayer::ProcessHit(const int nValue)
{
	bool bHit = false;

	// 体力取得
	int nLife = GetLife();

	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	if (m_state == STATE_COUNTER ||
		m_state == STATE_AVOID)
	{// ダメージ受けない状態
		return false;
	}

	if (m_state != STATE_DMG &&
		m_state != STATE_KNOCKBACK &&
		m_state != STATE_INVINCIBLE &&
		m_state != STATE_DEAD &&
		m_state != STATE_FADEOUT)
	{// ダメージ受付状態の時

		// 当たった
		bHit = true;

		// 体力減らす
		nLife -= nValue;

		// ゲームパッド情報取得
		CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();
		pInputGamepad->SetVibration(CInputGamepad::VIBRATION_STATE_DMG, 0);

		m_KnokBackMove.y += 18.0f;
		m_bHitStage = false;

		// 体力設定
		SetLife(nLife);

		if (nLife <= 0)
		{// 体力がなくなったら

			// 死状態
			m_state = STATE_DEAD;

			m_KnokBackMove.y = 8.0f;

			// 遷移カウンター設定
			m_nCntState = DEADTIME;

			// 体力設定
			SetLife(0);

			// ノックバック判定にする
			m_sMotionFrag.bKnockBack = true;

			// やられモーション
			//pMotion->Set(MOTION_KNOCKBACK);

			// ノックバックの位置更新
			MyLib::Vector3 pos = GetPosition();
			MyLib::Vector3 rot = GetRotation();
			m_posKnokBack = pos;

			// 衝撃波生成
			CImpactWave::Create
			(
				MyLib::Vector3(pos.x, pos.y + 80.0f, pos.z),	// 位置
				MyLib::Vector3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// 向き
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f),			// 色
				80.0f,										// 幅
				80.0f,										// 高さ
				0.0f,										// 中心からの間隔
				20,											// 寿命
				10.0f,										// 幅の移動量
				CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
				false										// 加算合成するか
			);

			CManager::GetInstance()->SetEnableHitStop(18);

			// 振動
			pCamera->SetShake(21, 30.0f, 0.0f);

			// 死んだ
			return true;
		}

		// 過去の状態保存
		m_Oldstate = m_state;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		// ダメージ状態にする
		m_state = STATE_DMG;

		// 遷移カウンター設定
		m_nCntState = 0;

		// ノックバックの位置更新
		MyLib::Vector3 pos = GetPosition();
		MyLib::Vector3 rot = GetRotation();
		m_posKnokBack = pos;

		// ノックバック判定にする
		m_sMotionFrag.bKnockBack = true;

		// やられモーション
		GetMotion()->Set(MOTION_KNOCKBACK);

		// 衝撃波生成
		CImpactWave::Create
		(
			MyLib::Vector3(pos.x, pos.y + 80.0f, pos.z),	// 位置
			MyLib::Vector3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// 向き
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f),			// 色
			80.0f,										// 幅
			80.0f,										// 高さ
			0.0f,										// 中心からの間隔
			20,											// 寿命
			10.0f,										// 幅の移動量
			CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
			false										// 加算合成するか
		);

		CManager::GetInstance()->SetEnableHitStop(12);

		// 振動
		pCamera->SetShake(12, 25.0f, 0.0f);

		// サウンド再生
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_PLAYERDMG);
	}

	return bHit;
}

//==========================================================================
// なし
//==========================================================================
void CPlayer::StateNone(void)
{
	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

//==========================================================================
// 無敵状態
//==========================================================================
void CPlayer::StateInvincible(void)
{
	// 状態遷移カウンター減算
	m_nCntState--;

	if (m_nCntState % INVINCIBLE_INT == 0)
	{// 規定間隔
		
		// 色設定
		if (m_mMatcol.a == 1.0f)
		{
			m_mMatcol.a = 0.4f;
		}
		else
		{
			m_mMatcol.a = 1.0f;
		}
	}

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら

		// なにもない状態にする
		m_state = STATE_NONE;
		m_nCntState = 0;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

}

//==========================================================================
// ダメージ状態
//==========================================================================
void CPlayer::StateDamage(void)
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// 向き取得
	MyLib::Vector3 rot = GetRotation();

	// 状態遷移カウンター減算
	m_nCntState++;

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	// 位置更新
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + m_KnokBackMove.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;

	// 起伏との判定
	if ((CManager::GetInstance()->GetScene()->GetElevation()->IsHit(pos) || m_bHitStage) && m_nCntState >= 10)
	{// 地面と当たっていたら
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		m_KnokBackMove.y = 0.0f;	// 移動量ゼロ
		m_bLandOld = true;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// ノックバック判定消す
		m_sMotionFrag.bKnockBack = false;

		// モーション取得
		CMotion* pMotion = GetMotion();
		if (pMotion == NULL)
		{// モーションがNULLだったら
			return;
		}
		pMotion->ToggleFinish(true);


		// Xファイルとの判定
		CStage *pStage = CGame::GetInstance()->GetStage();
		if (pStage == NULL)
		{// NULLだったら
			return;
		}

		// ステージに当たった判定
		for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
		{
			// オブジェクト取得
			CObjectX *pObjX = pStage->GetObj(nCntStage);

			if (pObjX == NULL)
			{// NULLだったら
				continue;
			}

			// 高さ取得
			bool bLand = false;
			pos.y = pObjX->GetHeight(pos, bLand);
		}


	}
	else if (m_nCntState >= 30)
	{// 遷移カウンターが30になったら

		// ノックバック状態にする
		m_state = STATE_KNOCKBACK;
	}


	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// 死亡状態
//==========================================================================
void CPlayer::StateDead(void)
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// 位置更新
	if (m_nCntState > 0)
	{
		int nCnt = DEADTIME - m_nCntState;
		pos.y = (-0.1f * (float)(nCnt * nCnt) + m_KnokBackMove.y * (float)nCnt) + m_posKnokBack.y;
		pos.x += move.x;
	}

	// 状態遷移カウンター減算
	m_nCntState--;

	// 起伏との判定
	if ((CManager::GetInstance()->GetScene()->GetElevation()->IsHit(pos) || m_bHitStage) && m_nCntState >= 10)
	{// 地面と当たっていたら

		// 死亡マネージャ生成
		CDeadManager::Create();

		m_state = STATE_DEADWAIT;	// 死亡待機状態
		m_nCntState = FADEOUTTIME;
		m_KnokBackMove.y = 0.0f;	// 移動量ゼロ
		m_bLandOld = true;
		move.x = 0.0f;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// ノックバック判定消す
		m_sMotionFrag.bKnockBack = false;
		m_sMotionFrag.bDead = true;
		//pMotion->ToggleFinish(true);

		// ぶっ倒れモーション
		//pMotion->Set(MOTION_DEAD);

		// Xファイルとの判定
		CStage *pStage = CGame::GetInstance()->GetStage();
		if (pStage == NULL)
		{// NULLだったら
			return;
		}

		// ステージに当たった判定
		for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
		{
			// オブジェクト取得
			CObjectX *pObjX = pStage->GetObj(nCntStage);

			if (pObjX == NULL)
			{// NULLだったら
				continue;
			}

			// 高さ取得
			bool bLand = false;
			pos.y = pObjX->GetHeight(pos, bLand);
		}
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// 死亡待機
//==========================================================================
void CPlayer::StateDeadWait(void)
{
	// ぶっ倒れモーション
	GetMotion()->Set(MOTION_DEAD);
}

//==========================================================================
// フェードアウト状態
//==========================================================================
void CPlayer::StateFadeOut(void)
{
	// 状態遷移カウンター減算
	m_nCntState--;

	// 色設定
	m_mMatcol.a = (float)m_nCntState / (float)FADEOUTTIME;

	// ぶっ倒れモーション
	GetMotion()->Set(MOTION_DEAD);

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら

		// 死亡処理
		Kill();

		// 終了処理
		Uninit();
		return;
	}
}

//==========================================================================
// ノックバック
//==========================================================================
void CPlayer::StateKnockBack(void)
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// 向き取得
	MyLib::Vector3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// 移動量取得
	float fMove = GetVelocity();

	// 現在と目標の差分
	float fRotDiff = 0.0f;

	// 距離の判定
	bool bLen = false;


	// 状態遷移カウンター減算
	m_nCntState++;

	// 位置更新
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + m_KnokBackMove.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;

	// 起伏との判定
	if ((CManager::GetInstance()->GetScene()->GetElevation()->IsHit(pos) || m_bHitStage))
	{// 地面と当たっていたら
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		m_KnokBackMove.y = 0.0f;	// 移動量ゼロ
		m_bLandOld = true;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		
		// ノックバック判定消す
		m_sMotionFrag.bKnockBack = false;

		// モーション取得
		CMotion* pMotion = GetMotion();
		if (pMotion == NULL)
		{// モーションがNULLだったら
			return;
		}
		pMotion->ToggleFinish(true);

		// Xファイルとの判定
		CStage *pStage = CGame::GetInstance()->GetStage();
		if (pStage == NULL)
		{// NULLだったら
			return;
		}

		// ステージに当たった判定
		for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
		{
			// オブジェクト取得
			CObjectX *pObjX = pStage->GetObj(nCntStage);

			if (pObjX == NULL)
			{// NULLだったら
				continue;
			}

			// 高さ取得
			bool bLand = false;
			pos.y = pObjX->GetHeight(pos, bLand);
		}
	}


	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// リスポーン
//==========================================================================
void CPlayer::StateRespawn(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType != MOTION_RESPAWN)
	{// 復活が終了
		m_state = STATE_NONE;
		return;
	}

	if (nType != MOTION_RESPAWN)
	{
		// 復活モーション設定
		pMotion->Set(MOTION_RESPAWN);
	}
}

//==========================================================================
// カウンター状態
//==========================================================================
void CPlayer::StateCounter(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	CEnemy* pEnemy = CEnemy::GetListObj().GetData(m_nIdxRockOn);
	if (pEnemy == nullptr)
	{
		// 終了時の設定
		if (m_pEndCounterSetting != nullptr)
		{
			m_pEndCounterSetting->EndSetting(this);
			delete m_pEndCounterSetting;
			m_pEndCounterSetting = nullptr;
		}
		return;
	}

	MyLib::Vector3 enemypos = pEnemy->GetPosition();

	SetRotDest(atan2f((pos.x - enemypos.x), (pos.z - enemypos.z)));

	int nType = pMotion->GetType();
	if (nType != MOTION_COUNTER_ACCEPT &&
		nType != MOTION_COUNTER_TURN &&
		nType != MOTION_COUNTER_ATTACK)
	{// カウンター状態が終了

		// 終了時の設定
		if (m_pEndCounterSetting != nullptr)
		{
			m_pEndCounterSetting->EndSetting(this);
			delete m_pEndCounterSetting;
			m_pEndCounterSetting = nullptr;
		}
		return;
	}
}

//==========================================================================
// 回避
//==========================================================================
void CPlayer::StateAvoid(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType != MOTION_AVOID)
	{// 回避が終了
		m_state = STATE_NONE;
		return;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CPlayer::Draw(void)
{

	// 描画処理
	if (m_state == STATE_DMG)
	{
		CObjectChara::Draw(m_mMatcol);
	}
	else if (m_state == STATE_INVINCIBLE || m_state == STATE_FADEOUT)
	{
		CObjectChara::Draw(m_mMatcol.a);
	}
	else
	{
		CObjectChara::Draw();
	}
}

//==========================================================================
// 状態設定
//==========================================================================
void CPlayer::SetState(STATE state, int nCntState)
{
	m_state = state;
	m_nCntState = nCntState;
}

//==========================================================================
// 状態取得
//==========================================================================
CPlayer::STATE CPlayer::GetState(void)
{
	return m_state;
}

//==========================================================================
// 体力アップグレード
//==========================================================================
void CPlayer::UpgradeLife(int addvalue)
{
	// アップグレード後の体力に設定
	SetLife(m_pHPGauge->UpgradeMaxValue(addvalue));
	SetLifeOrigin(m_pHPGauge->GetMaxLife());
}

//==========================================================================
// スタミナアップグレード
//==========================================================================
void CPlayer::UpgradeMaxStamina(int addvalue)
{
	// スタミナ最大値上昇
	if (m_pStaminaGauge != nullptr)
	{
		m_pStaminaGauge->UpgradeMaxValue(addvalue);
	}
}

//==========================================================================
// スタミナ自動回復アップグレード
//==========================================================================
void CPlayer::UpgradeAutoHealStamina(float ratio)
{
	// スタミナ自動回復値上昇
	if (m_pStaminaGauge != nullptr)
	{
		m_pStaminaGauge->UpgradeAutoHeal(ratio);
	}
}

//==========================================================================
// リスポーン時の設定
//==========================================================================
void CPlayer::RespawnSetting()
{
	SetMotion(CPlayer::MOTION_RESPAWN);
	m_state = STATE_RESPAWN;

	m_nRespawnPercent -= 30;
	UtilFunc::Transformation::ValueNormalize(m_nRespawnPercent, 100, 0);

	SetLife(GetLifeOrigin());
}

//==========================================================================
// リスポーン確率設定
//==========================================================================
void CPlayer::SetRespawnPercent(int value)
{
	m_nRespawnPercent = value;
	UtilFunc::Transformation::ValueNormalize(m_nRespawnPercent, 100, 0);
}


//==========================================================================
// カウンター終了時の設定
//==========================================================================
void CEndCounterSetting::EndSetting(CPlayer* player)
{
	player->SetState(CPlayer::STATE_NONE);
	CManager::GetInstance()->GetCamera()->SetRockOnState(CCamera::RockOnState::ROCKON_NORMAL);
}

void CEndAttack::EndSetting(CPlayer* player)
{
	// 親の処理
	CEndCounterSetting::EndSetting(player);

	if (!player->IsLockOnAtStart())
	{
		if (CManager::GetInstance()->GetCamera()->IsRockOn())
		{// ロックオン解除

			// ロックオン設定
			CManager::GetInstance()->GetCamera()->SetRockOn(0.0f, false);

			// リストループ
			CListManager<CEnemy> enemyList = CEnemy::GetListObj();
			CEnemy* pEnemy = nullptr;
			while (enemyList.ListLoop(&pEnemy))
			{
				pEnemy->SetEnableRockOn(false);
			}
		}
	}
}