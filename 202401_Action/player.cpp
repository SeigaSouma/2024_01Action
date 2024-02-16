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
#include "gallery.h"
#include "skilltree_obj.h"

#include "playercontrol.h"

struct sPlayerStatus;

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* CHARAFILE = "data\\TEXT\\character\\player\\tyuuni\\setup_player.txt";	// キャラクターファイル
	const float JUMP = 20.0f * 1.5f;			// ジャンプ力初期値
	const int TIME_DMG = 20;					// ダメージ時間
	const int INVINCIBLE_INT = 2;				// 無敵の間隔
	const int INVINCIBLE_TIME = 0;				// 無敵の時間
	const int DEADTIME = 120;					// 死亡時の時間
	const int FADEOUTTIME = 60;					// フェードアウトの時間
	const float MULTIPLIY_DASH = 1.875f;		// ダッシュの倍率
	const float TIME_DASHATTACK = 0.3f;			// ダッシュ攻撃に必要な時間
	const int DEFAULT_STAMINA = 200;			// スタミナのデフォルト値
	const float SUBVALUE_DASH = 0.1f;			// ダッシュの減算量
	const float SUBVALUE_AVOID = 30.0f;			// 回避の減算量

	// ステータス
	const float DEFAULT_RESPAWNHEAL = 0.3f;				// リスポーン時の回復割合
	const float DEFAULT_SUBVALUE_GUARD = 60.0f;			// ガードのスタミナ減算量
	const float DEFAULT_SUBVALUE_COUNTER = 40.0f;		// カウンターのスタミナ減算量
	const float DEFAULT_COUNTERHEAL = 0.0f;				// カウンターのスタミナ回復量
	const float DEFAULT_MULTIPLY_ATTACK = 1.0f;			// 攻撃倍率
	const float DEFAULT_CHARGETIME = 0.9f;				// チャージ時間
	const int DEFAULT_FRAME_EXTENSION_COUNTER = 16;	// カウンター猶予フレーム
	const float  DEFAULT_MULTIPLY_GUARD = 0.4f;			// カードの軽減
	const float DEFAULT_TIME_ADDDOWN = 2.5f;			// ダウン時間付与
	const bool DEFAULT_IS_CHARGEFLINCH = true;			// チャージ時怯みフラグ
	const int DEFAULT_RESPAWN_PERCENT = 20;				// 復活確率
	const float MULTIPLY_CHARGEATK = 2.0f;				// チャージ攻撃の倍率
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
	&CPlayer::StateDown,		// ダウン
	&CPlayer::StateDead,		// 死亡
	&CPlayer::StateDeadWait,	// 死亡待機
	&CPlayer::StateFadeOut,		// フェードアウト
	&CPlayer::StateRespawn,		// リスポーン
	&CPlayer::StateCounter,		// カウンター中
	&CPlayer::StateAvoid,		// 回避
	&CPlayer::StatePrayer,		// 祈り
	&CPlayer::StateCharge,		// チャージ
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
	m_bReadyDashAtk = false;						// ダッシュアタックのフラグ
	m_bAttacking = false;							// 攻撃中
	m_bCounterAccepting = false;					// カウンター受付中
	m_bDash = false;								// ダッシュ判定
	m_fDashTime = 0.0f;								// ダッシュ時間
	m_fChargeTime = 0.0f;							// チャージ時間
	m_bChargeCompletion = false;					// チャージ完了フラグ
	m_nRespawnPercent = 0;							// リスポーン確率
	m_bTouchBeacon = false;							// ビーコンに触れてる判定

	m_PlayerStatus = sPlayerStatus();				// プレイヤーステータス

	m_nMyPlayerIdx = 0;								// プレイヤーインデックス番号
	m_pShadow = nullptr;								// 影の情報
	m_pSkillPoint = nullptr;						// スキルポイントのオブジェクト
	m_pHPGauge = nullptr;							// HPゲージのポインタ
	m_pStaminaGauge = nullptr;						// スタミナゲージのポインタ

	m_pEndCounterSetting = nullptr;					// カウンター終了時の設定
	m_pControlAtk = nullptr;						// 攻撃操作
	m_pControlDefence = nullptr;					// 防御操作
	m_pControlAvoid = nullptr;						// 回避操作
	m_pGuard = nullptr;								// ガード

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
	CPlayer *pPlayer = nullptr;

	if (pPlayer == nullptr)
	{// nullptrだったら

		// メモリの確保
		pPlayer = DEBUG_NEW CPlayer;

		if (pPlayer != nullptr)
		{// メモリの確保が出来ていたら

			// プレイヤーインデックス番号
			pPlayer->m_nMyPlayerIdx = nIdx;

			// 初期化処理
			pPlayer->Init();
		}

		return pPlayer;
	}

	return nullptr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CPlayer::Init()
{
	// 種類の設定
	SetType(TYPE_PLAYER);

	m_state = STATE_NONE;	// 状態
	m_nCntState = 0;		// 状態遷移カウンター
	m_bLandOld = true;		// 前回の着地状態
	m_nRespawnPercent = DEFAULT_RESPAWN_PERCENT;	// リスポーン確率

	// 強化リセット
	ResetEnhance();

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

	//// 操作関数
	//ChangeAtkControl(DEBUG_NEW CPlayerControlAttack);	// 攻撃操作
	//ChangeDefenceControl(DEBUG_NEW CPlayerControlDefence);	// 防御操作
	//ChangeAvoidControl(DEBUG_NEW CPlayerControlAvoid);	// 回避操作

	// ガード
	ChangeGuardGrade(DEBUG_NEW CPlayerGuard);

	return S_OK;
}

//==========================================================================
// 攻撃の操作変更
//==========================================================================
void CPlayer::ChangeAtkControl(CPlayerControlAttack* control)
{ 
	delete m_pControlAtk;
	m_pControlAtk = control;
}

//==========================================================================
// 防御の操作変更
//==========================================================================
void CPlayer::ChangeDefenceControl(CPlayerControlDefence* control)
{ 
	delete m_pControlDefence;
	m_pControlDefence = control;
}

//==========================================================================
// 回避の操作変更
//==========================================================================
void CPlayer::ChangeAvoidControl(CPlayerControlAvoid* control)
{ 
	delete m_pControlAvoid;
	m_pControlAvoid = control;
}

//==========================================================================
// ガード性能変更
//==========================================================================
void CPlayer::ChangeGuardGrade(CPlayerGuard* guard)
{
	delete m_pGuard;
	m_pGuard = guard;
}

//==========================================================================
// 終了処理
//==========================================================================
void CPlayer::Uninit()
{
	// 影を消す
	if (m_pShadow != nullptr)
	{
		//m_pShadow->Uninit();
		m_pShadow = nullptr;
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

	// 操作系
	if (m_pControlAtk != nullptr)
	{
		delete m_pControlAtk;
		m_pControlAtk = nullptr;
	}
	if (m_pControlDefence != nullptr)
	{
		delete m_pControlDefence;
		m_pControlDefence = nullptr;
	}
	if (m_pControlAvoid != nullptr)
	{
		delete m_pControlAvoid;
		m_pControlAvoid = nullptr;
	}
	if (m_pGuard != nullptr)
	{
		delete m_pGuard;
		m_pGuard = nullptr;
	}

	// 終了処理
	CObjectChara::Uninit();

	// 削除
	m_List.Delete(this);
}

//==========================================================================
// 終了処理
//==========================================================================
void CPlayer::Kill()
{
	// 遷移なしフェード追加
	CManager::GetInstance()->GetInstantFade()->SetFade(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), 40);

	// 遷移状態に変更
	CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_REASPAWN);

	my_particle::Create(GetPosition(), my_particle::TYPE_ENEMY_FADE);

	if (m_pSkillPoint != nullptr)
	{
		m_pSkillPoint->Kill();
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
	if (m_pShadow != nullptr)
	{
		m_pShadow->Uninit();
		m_pShadow = nullptr;
	}

	// 反撃終了時の設定
	if (m_pEndCounterSetting != nullptr)
	{
		delete m_pEndCounterSetting;
		m_pEndCounterSetting = nullptr;
	}

	// 操作系
	if (m_pControlAtk != nullptr)
	{
		delete m_pControlAtk;
		m_pControlAtk = nullptr;
	}
	if (m_pControlDefence != nullptr)
	{
		delete m_pControlDefence;
		m_pControlDefence = nullptr;
	}
	if (m_pControlAvoid != nullptr)
	{
		delete m_pControlAvoid;
		m_pControlAvoid = nullptr;
	}
	if (m_pGuard != nullptr)
	{
		delete m_pGuard;
		m_pGuard = nullptr;
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
void CPlayer::Update()
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
	if (pCamera->GetStateCameraR() == CCamera::POSR_STATE_ROCKON)
	{
		pCamera->SetTargetPosition(pos);
		pCamera->SetTargetRotation(rot);
	}
	else if(pCamera->GetStateCameraR() == CCamera::POSR_STATE_NORMAL)
	{
		MyLib::Vector3 camerapos = pos;
		//camerapos.y = pCamera->GetTargetPosition().y;

		pCamera->SetTargetPosition(camerapos);
		pCamera->SetTargetRotation(rot);
	}

	// 影の位置更新
	if (m_pShadow != nullptr)
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
		if (pEffect == nullptr)
		{// nullptrだったら
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

	GetCenterPosition();

#if _DEBUG
	// デバッグ表示
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[プレイヤーの操作]------------------\n"
		"位置：【X：%f, Y：%f, Z：%f】 【W / A / S / D】\n"
		"向き：【X：%f, Y：%f, Z：%f】 【Z / C】\n"
		"移動量：【X：%f, Y：%f, Z：%f】\n"
		"体力：【%d】\n"
		"状態：【%d】\n"
		"コンボステージ：【%d】\n"
		"ダッシュ時間：【%f】\n"
		, pos.x, pos.y, pos.z, rot.x, rot.y, rot.y, move.x, move.y, move.z, GetLife(), m_state, m_nComboStage, m_fDashTime);

#endif

}

//==========================================================================
// 操作処理
//==========================================================================
void CPlayer::Controll()
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

		// 操作関数
		if (m_state != STATE_KNOCKBACK)
		{
			m_pControlAtk->Attack(this);		// 攻撃操作
			m_pControlDefence->Defence(this);	// 防御操作
			m_pControlAvoid->Avoid(this);		// 回避操作
		}
		nMotionType = pMotion->GetType();
		fRotDest = GetRotDest();

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
			!m_pControlAtk->IsReserve() &&
			//!m_sMotionFrag.bATK &&
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

			if (m_bJump == false &&
				(pInputKeyboard->GetTrigger(DIK_SPACE) == true ||
					pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, m_nMyPlayerIdx)) &&
				!m_bTouchBeacon)
			{// ジャンプ

				m_bJump = true;
				m_sMotionFrag.bJump = true;
				move.y += 17.0f;

				pMotion->Set(MOTION_JUMP);

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

		}

		// 角度の正規化
		UtilFunc::Transformation::RotNormalize(fRotDest);
		SetRotDest(fRotDest);
	}

	// 移動量加算
	newPosition.x += move.x;
	newPosition.z += move.z;

	sakiPos.x = newPosition.x + sinf(D3DX_PI + rot.y) * GetRadius();
	sakiPos.z = newPosition.z + cosf(D3DX_PI + rot.y) * GetRadius();

	// 現在と目標の差分を求める
	float fRotDiff = GetRotDest() - rot.y;

	// 角度の正規化
	UtilFunc::Transformation::RotNormalize(fRotDiff);

	// 角度の補正をする
	rot.y += fRotDiff * 0.25f;
	UtilFunc::Transformation::RotNormalize(rot.y);

	// 向き設定
	SetRotation(rot);


	// 重力処理
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DEAD && m_state != STATE_FADEOUT && m_state != STATE_DEADWAIT)
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
		pos.y -= mylib_const::GRAVITY * 7.0f;

		if (m_bJump == false)
		{
			m_bJump = true;
		}

		Collision(pos, move);
	}

	// 慣性補正
	if (m_state == STATE_DMG)
	{
		move.x += (0.0f - move.x) * 0.1f;
		move.z += (0.0f - move.z) * 0.1f;
	}
	else if (m_state != STATE_KNOCKBACK && m_state != STATE_DEAD && m_state != STATE_FADEOUT)
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

#if _DEBUG
	if (pInputKeyboard->GetTrigger(DIK_LEFT) == true)
	{
		CCollisionObject::Create(GetPosition(), mylib_const::DEFAULT_VECTOR3, 100000.0f, 3, 10000, CCollisionObject::TAG_PLAYER);
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{
		//m_sMotionFrag.bATK = true;		// 攻撃判定OFF

		MyLib::Vector3 weponpos = pos;

		//CMyEffekseer::GetInstance()->SetEffect(
		//	&m_pWeaponHandle,
		//	"data/Effekseer/debugline_green.efkefc",
		//	weponpos, rot, 0.0f, 40.0f, true);

		// デバッグ表示
		CManager::GetInstance()->GetDebugProc()->Print(
			"わああああああああああああああああああああああああああああああああああああああああああああ\n");
	}

	if (pInputKeyboard->GetRepeat(DIK_O, 4) == true)
	{
		MyLib::Vector3 weponpos = pos;
		weponpos.y += 150.0f;

		MyLib::Vector3 spawnpos = UtilFunc::Transformation::GetRandomPositionSphere(weponpos, 300.0f);
		m_pSkillPoint->AddPoint();
	}

	static float fff = 1.0f;
	if (pInputKeyboard->GetTrigger(DIK_UP) == true)
	{
		fff += 0.1f;
		CManager::GetInstance()->GetSound()->SetFrequency(CSound::LABEL_BGM_GAME, fff);

		MyLib::Vector3 weponpos = 0.0f;
		weponpos.y += 150.0f;
		weponpos.z += 800.0f;
		CMyEffekseer::GetInstance()->SetEffect(
			CMyEffekseer::EFKLABEL_COUNTERLINE2,
			weponpos, 0.0f, 0.0f, 60.0f);

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
#endif
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
void CPlayer::MotionSet()
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	if (m_state == STATE_DEAD ||
		m_state == STATE_DEADWAIT ||
		m_state == STATE_DOWN ||
		m_state == STATE_CHARGE ||
		m_state == STATE_KNOCKBACK ||
		m_state == STATE_PRAYER)
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
void CPlayer::MotionBySetState()
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
		m_pStaminaGauge->SubValue(SUBVALUE_DASH);
		break;

	default:
		m_fDashTime = 0.0f;
		break;
	}

	// ダッシュアタックのフラグ
	if (m_fDashTime >= TIME_DASHATTACK)
	{
		m_bReadyDashAtk = true;
	}
	else
	{
		m_bReadyDashAtk = false;
	}

	// ガードフラグ
	switch (nType)
	{
	case MOTION_GUARD:
	case MOTION_GUARD_DMG:
		m_sMotionFrag.bGuard = true;	// ガードON

		if (m_pStaminaGauge->GetState() == CStaminaGauge_Player::STATE_NORMAL)
		{
			m_pStaminaGauge->SetValue(m_pStaminaGauge->GetValue() - m_pStaminaGauge->GetAutoHealValue());
		}
		break;

	default:
		m_sMotionFrag.bGuard = false;	// ガードOFF
		break;
	}

	// 攻撃フラグ
	switch (nType)
	{
	case MOTION_ATK:
	case MOTION_ATK2:
	case MOTION_ATK3:
	case MOTION_ATK4:
	case MOTION_ATK4_FINISH:
	case MOTION_DASHATK:
		m_bAttacking = true;
		break;

	case MOTION_COUNTER_TURN:
	case MOTION_COUNTER_ATTACK:
		// カウンター状態に設定
		m_state = STATE_COUNTER;
		break;

	default:
		// チャージ完了フラグ
		m_bChargeCompletion = false;

		m_bAttacking = false;	// 攻撃中判定
		m_sMotionFrag.bATK = false;
		m_nComboStage = 0;		// コンボの段階
		break;
	}


	switch (nType)
	{
	case MOTION_COUNTER_ACCEPT:

		if (pMotion->GetAllCount() <= m_PlayerStatus.counterExtensionFrame)
		{
			m_bCounterAccepting = true;	// カウンター受付中
		}
		else
		{
			m_bCounterAccepting = false;	// カウンター受付中

		}
		break;

	default:
		m_bCounterAccepting = false;	// カウンター受付中
		break;
	}

	// インプット情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// チャージ移行
	switch (nType)
	{
	case MOTION_ATK4:
		if (pInputGamepad->GetPress(CInputGamepad::BUTTON_Y, m_nMyPlayerIdx))
		{
			m_state = STATE_CHARGE;
		}
		break;

	default:
		m_fChargeTime = 0.0f;
		break;
	}

	// 回避
	if (nType == MOTION_AVOID &&
		!m_bInDicision)
	{
		m_state = STATE_NONE;
	}
}

//==========================================================================
// フラグリセット
//==========================================================================
void CPlayer::ResetFrag()
{
	m_bCounterAccepting = false;	// カウンター受付中
}

//==========================================================================
// ロックオン
//==========================================================================
void CPlayer::RockOn()
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
		//pCamera->SetRockOn(enemyList.GetData(nMaxIdx)->GetPosition(), true);
		enemyList.GetData(nMaxIdx)->SetEnableRockOn(true);

		// インデックス番号設定
		m_nIdxRockOn = nMaxIdx;
	}
}

//==========================================================================
// ロックオン対象切り替え
//==========================================================================
void CPlayer::SwitchRockOnTarget()
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

		if (enemyList.GetData(m_nIdxRockOn) != nullptr)
		{
			// 今までロックオンしてた対象リセット
			enemyList.GetData(m_nIdxRockOn)->SetEnableRockOn(false);
		}

		// ロックオン設定
		CEnemy* pSetEnemy = enemyList.GetData(nMaxIdx);
		//CManager::GetInstance()->GetCamera()->SetRockOn(pSetEnemy->GetPosition(), true);
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
	MyLib::Vector3 rot = GetRotation();

	switch (nType)
	{
	case MOTION_ATK:
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_SNOWGET);
		break;

	case MOTION_WALK:
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_WALK);
		break;

	case MOTION_COUNTER_TURN:
		CMyEffekseer::GetInstance()->SetEffect(
			CMyEffekseer::EFKLABEL_COUNTERLINE,
			weponpos, 0.0f, 0.0f, 50.0f);

		CMyEffekseer::GetInstance()->SetEffect(
			CMyEffekseer::EFKLABEL_COUNTERLINE2,
			weponpos, MyLib::Vector3(0.0f, D3DX_PI + rot.y, 0.0f), 0.0f, 40.0f);

		CMyEffekseer::GetInstance()->SetEffect(
			CMyEffekseer::EFKLABEL_COUNTER_KRKR,
			GetPosition(), 0.0f, 0.0f, 40.0f);
		break;

	case MOTION_COUNTER_ATTACK:

		if (nCntATK == 0)
		{
			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_COUNTERLINE,
				weponpos, 0.0f, 0.0f, 50.0f);

			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_COUNTERLINE2,
				weponpos, 0.0f, 0.0f, 40.0f);

			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_COUNTER_KRKR,
				GetPosition(), 0.0f, 0.0f, 40.0f);
		}
		else if (nCntATK != 0)
		{
			CManager::GetInstance()->GetCamera()->SetLenDest(200.0f, 3, 4.0f, 0.3f);

			// 敵へダウン状態
			CEnemy* pEnemy = CEnemy::GetListObj().GetData(m_nIdxRockOn);

			if (pEnemy != nullptr)
			{
				pEnemy->Hit(static_cast<int>(10.0f * m_PlayerStatus.attackMultiply), CGameManager::ATTACK_COUNTER);
				pEnemy->SetDownTime(m_PlayerStatus.addDownTime);
			}
		}
		break;

	case MOTION_PRAYER:
		CSkillTree_Obj::GetInstance()->StartUp();
		break;

	default:
		break;
	}
}

//==========================================================================
// 攻撃判定中処理
//==========================================================================
void CPlayer::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
{
	// モーション取得
	CMotion* pMotion = GetMotion();

	// 武器の位置
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), *pATKInfo);

	CEffect3D* pEffect = nullptr;

	switch (pMotion->GetType())
	{
	case MOTION_ATK:
		break;

	case MOTION_COUNTER_ACCEPT:
		m_bCounterAccepting = true;	// カウンター受付中
		break;

	case MOTION_AVOID:
		m_state = STATE_AVOID;
		break;

	case MOTION_PRAYER:
		MyLib::Vector3 objpos = CSkillTree_Obj::GetInstance()->GetPosition();
		MyLib::Vector3 pos = GetPosition();

		UtilFunc::Correction::InertiaCorrection(pos.x, 0.0f, 0.1f);
		UtilFunc::Correction::InertiaCorrection(pos.z, objpos.z - 20.0f, 0.1f);

		SetRotDest(pos.AngleXZ(objpos));
		SetPosition(pos);
		break;
	}

	if (pATKInfo->fRangeSize == 0.0f)
	{
		return;
	}

#if _DEBUG
	CEffect3D::Create(
		weponpos,
		MyLib::Vector3(0.0f, 0.0f, 0.0f),
		D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),
		pATKInfo->fRangeSize, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
#endif

	// 敵のリスト取得
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	// リストループ
	while (enemyList.ListLoop(&pEnemy))
	{
		// コライダーの数繰り返し
		std::vector<SphereCollider> colliders = pEnemy->GetSphereColliders();
		for (const auto& collider : colliders)
		{
			MyLib::HitResult hitresult = UtilFunc::Collision::SphereRange(weponpos, collider.center, pATKInfo->fRangeSize, collider.radius);
			if (hitresult.ishit)
			{// 球の判定

				int damage = static_cast<int>(static_cast<float>(pATKInfo->nDamage) * m_PlayerStatus.attackMultiply);

				if (pMotion->GetType() == MOTION_ATK4_FINISH &&
					m_bChargeCompletion)
				{
					damage *= MULTIPLY_CHARGEATK;
				}

				if (pEnemy->Hit(damage) == true)
				{// 当たってたら

					// 位置
					MyLib::Vector3 pos = GetPosition();
					MyLib::Vector3 enemypos = pEnemy->GetPosition();

					if (pEnemy->GetType() != CEnemy::TYPE_BOSS)
					{
						// ターゲットと敵との向き
						float fRot = atan2f((enemypos.x - pos.x), (enemypos.z - pos.z));
						UtilFunc::Transformation::RotNormalize(fRot);

						pEnemy->SetMove(MyLib::Vector3(sinf(fRot) * 8.0f, 0.0f, cosf(fRot) * 8.0f));
					}

					CMyEffekseer::GetInstance()->SetEffect(
						CMyEffekseer::EFKLABEL_HITMARK_RED,
						hitresult.hitpos, 0.0f, 0.0f, 50.0f);

					// ダメージ表記
					enemypos.y += pEnemy->GetHeight() * 0.5f;
					enemypos += UtilFunc::Transformation::GetRandomPositionSphere(enemypos, collider.radius * 0.5f);
					CDamagePoint::Create(hitresult.hitpos, damage);
					break;
				}
			}
		}
	}
}

//==========================================================================
// 位置制限
//==========================================================================
void CPlayer::LimitPos()
{
	MyLib::Vector3 pos = GetPosition();

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

	// 円の押し戻し
	if (pos.LengthXZ() > mylib_const::RADIUS_STAGE)
	{// 補正
		pos = pos.Normal() * mylib_const::RADIUS_STAGE;
	}
	SetPosition(pos);
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
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DEAD && m_state != STATE_FADEOUT)
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
	if (pStage == nullptr)
	{// nullptrだったら
		return false;
	}

	bool bNowLand = false;

	// ステージに当たった判定
	m_bHitStage = false;
	for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
	{
		// オブジェクト取得
		CObjectX *pObjX = pStage->GetObj(nCntStage);

		if (pObjX == nullptr)
		{// nullptrだったら
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

	// リストループ
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;
	MyLib::Vector3 enemypos;
	float radius = GetRadius();
	float enemyradius = 0.0f;

	while (enemyList.ListLoop(&pEnemy))
	{
		enemypos = pEnemy->GetCenterPosition();
		enemyradius = pEnemy->GetRadius();
		enemyradius *= 0.5f;

		if (UtilFunc::Collision::CircleRange3D(pos, enemypos, radius, enemyradius))
		{
			// ターゲットと敵との向き
			float fRot = atan2f((pos.x - enemypos.x), (pos.z - enemypos.z));
			float totalradius = radius + enemyradius;

			pos.x = enemypos.x + sinf(fRot) * totalradius;
			pos.z = enemypos.z + cosf(fRot) * totalradius;
		}
	}

	// 向き設定
	SetRotation(rot);

	return bNowLand;
}

//==========================================================================
// ヒット処理
//==========================================================================
MyLib::HitResult_Character CPlayer::Hit(const int nValue, CGameManager::AttackType atkType)
{
	MyLib::HitResult_Character hitresult = {};

	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	if (m_bCounterAccepting)
	{// カウンター受け付け中

		// 観衆のリスト取得
		CListManager<CGallery> galleryList = CGallery::GetList();
		CGallery* pGallery = nullptr;
		while (galleryList.ListLoop(&pGallery))
		{
			pGallery->SetState(CGallery::STATE_COUNTERHEAT);
		}
		
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

		// カウンターで回復
		m_pStaminaGauge->AddValue(m_PlayerStatus.counterStaminaHealValue);

		hitresult.ishit = true;
		return hitresult;
	}
	
	// 共通のヒット処理
	hitresult = ProcessHit(nValue, 0.0f);

	// 当たった判定を返す
	return hitresult;
}

//==========================================================================
// ヒット処理
//==========================================================================
MyLib::HitResult_Character CPlayer::Hit(const int nValue, CEnemy* pEnemy, CGameManager::AttackType atkType)
{

	MyLib::HitResult_Character hitresult = {};

	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	if (m_bCounterAccepting)
	{// カウンター受け付け中

		// 観衆のリスト取得
		CListManager<CGallery> galleryList = CGallery::GetList();
		CGallery* pGallery = nullptr;
		while (galleryList.ListLoop(&pGallery))
		{
			pGallery->SetState(CGallery::STATE_COUNTERHEAT);
		}

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
					//pCamera->SetRockOn(pEnemy->GetPosition(), true);
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

		// カウンターで回復
		m_pStaminaGauge->AddValue(m_PlayerStatus.counterStaminaHealValue);

		hitresult.ishit = true;
		return hitresult;
	}

	if (m_sMotionFrag.bGuard)
	{// ガード受け付け中

		// 共通のヒット処理
		if (atkType == CGameManager::ATTACK_STRONG)
		{// 強攻撃はガー不
			hitresult = ProcessHit(nValue, pEnemy->GetPosition());
			return hitresult;
		}

		if (GetMotion()->GetType() == MOTION_GUARD_DMG)
		{
			return hitresult;
		}

		if (GetMotion()->GetType() == MOTION_GUARD)
		{
			GetMotion()->Set(MOTION_GUARD_DMG);
		}

		// 体力設定
		int nLife = GetLife();
		int damage = nValue;
		nLife -= static_cast<int>(static_cast<float>(nValue) * m_PlayerStatus.damageMitigation);
		SetLife(nLife);

		if (nLife <= 0)
		{// 体力がなくなったら

			// 死亡時の設定
			DeadSetting(&hitresult);
			return hitresult;
		}

		// ヒット時の処理
		m_pGuard->HitProcess(this, pEnemy->GetPosition());
		hitresult.ishit = true;

		if (GetStaminaGauge()->GetValue() <= 0.0f)
		{
			// ダウン時の設定
			DownSetting(pEnemy->GetPosition());
			return hitresult;
		}
		return hitresult;
	}

	// 共通のヒット処理
	hitresult = ProcessHit(nValue, pEnemy->GetPosition());

	return hitresult;
}

//==========================================================================
// 共通のヒット処理
//==========================================================================
MyLib::HitResult_Character CPlayer::ProcessHit(const int nValue, const MyLib::Vector3& hitpos)
{

	MyLib::HitResult_Character hitresult = {};

	// 体力取得
	int nLife = GetLife();

	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	if (m_state == STATE_COUNTER ||
		m_state == STATE_AVOID)
	{// ダメージ受けない状態
		return hitresult;
	}

	if (m_state != STATE_DMG &&
		m_state != STATE_KNOCKBACK &&
		m_state != STATE_INVINCIBLE &&
		m_state != STATE_DEAD &&
		m_state != STATE_DEADWAIT &&
		m_state != STATE_RESPAWN &&
		m_state != STATE_FADEOUT)
	{// ダメージ受付状態の時

		// 当たった
		hitresult.ishit = true;
		hitresult.isdamage = true;

		// 体力減らす
		nLife -= nValue;

		// ゲームパッド情報取得
		CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();
		pInputGamepad->SetVibration(CInputGamepad::VIBRATION_STATE_DMG, 0);

		// ノックバックする時
		//m_KnokBackMove.y += 18.0f;
		m_bHitStage = false;

		// 体力設定
		SetLife(nLife);

		if (nLife <= 0)
		{// 体力がなくなったら

			// ノックバック判定にする
			m_sMotionFrag.bKnockBack = true;

			// ノックバック状態にする
			m_state = STATE_KNOCKBACK;

			DeadSetting(&hitresult);
			//m_KnokBackMove.y += 18.0f;
			return hitresult;
		}

		// 過去の状態保存
		m_Oldstate = m_state;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		// 遷移カウンター設定
		m_nCntState = 0;

		// ノックバックの位置更新
		MyLib::Vector3 pos = GetPosition();
		m_posKnokBack = pos;
		
		float hitAngle = pos.AngleXZ(hitpos);
		// 衝撃波生成
		CImpactWave::Create
		(
			MyLib::Vector3(pos.x, pos.y + 80.0f, pos.z),	// 位置
			MyLib::Vector3(D3DX_PI * 0.5f, D3DX_PI + hitAngle, D3DX_PI),				// 向き
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f),			// 色
			80.0f,										// 幅
			80.0f,										// 高さ
			0.0f,										// 中心からの間隔
			20,											// 寿命
			10.0f,										// 幅の移動量
			CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
			false										// 加算合成するか
		);


		if (GetStaminaGauge()->GetValue() <= 0.0f)
		{
			// ダウン時の設定
			DownSetting(hitpos);
			return hitresult;
		}
		else
		{
			// ダメージ状態にする
			m_state = STATE_DMG;

			MyLib::Vector3 move;
			move.x = sinf(D3DX_PI + hitAngle) * -10.0f;
			move.z = cosf(D3DX_PI + hitAngle) * -10.0f;
			SetMove(move);

			// やられモーション
			GetMotion()->Set(MOTION_DMG);
		}

		//CManager::GetInstance()->SetEnableHitStop(12);

		// 振動
		pCamera->SetShake(8, 18.0f, 0.0f);

		// サウンド再生
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_PLAYERDMG);
	}

	return hitresult;
}

//==========================================================================
// ダウン時の設定
//==========================================================================
void CPlayer::DownSetting(const MyLib::Vector3& hitpos)
{
	// ノックバック判定にする
	m_sMotionFrag.bKnockBack = true;

	// ノックバック状態にする
	m_state = STATE_KNOCKBACK;


	m_KnokBackMove.y = 10.0f;


	MyLib::Vector3 pos = GetPosition();

	// 移動量設定
	float hitAngle = pos.AngleXZ(hitpos);
	MyLib::Vector3 move;
	move.x = sinf(D3DX_PI + hitAngle) * -7.0f;
	move.z = cosf(D3DX_PI + hitAngle) * -7.0f;
	SetMove(move);

	// ノックバックモーション
	GetMotion()->Set(MOTION_KNOCKBACK);

	// 振動
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	if (pCamera != nullptr)
	{
		pCamera->SetShake(18, 25.0f, 0.0f);
	}
}

//==========================================================================
// 死亡時の設定
//==========================================================================
void CPlayer::DeadSetting(MyLib::HitResult_Character* result)
{
	CCamera* pCamera = CManager::GetInstance()->GetCamera();

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
	GetMotion()->Set(MOTION_KNOCKBACK);

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
	result->isdeath = true;
}

//==========================================================================
// なし
//==========================================================================
void CPlayer::StateNone()
{
	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

//==========================================================================
// 無敵状態
//==========================================================================
void CPlayer::StateInvincible()
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
void CPlayer::StateDamage()
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

	if (m_nCntState >= TIME_DMG)
	{
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;

		// モーション取得
		CMotion* pMotion = GetMotion();
		if (pMotion == nullptr)
		{
			return;
		}
		pMotion->ToggleFinish(true);
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// ノックバック
//==========================================================================
void CPlayer::StateKnockBack()
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

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}
	int nType = pMotion->GetType();

	if (nType == MOTION_KNOCKBACK)
	{
		// 状態遷移カウンター減算
		m_nCntState++;

		// 位置更新
		pos.y = (-0.3f * (float)(m_nCntState * m_nCntState) + m_KnokBackMove.y * (float)m_nCntState) + m_posKnokBack.y;
		pos.x += move.x;
		pos.z += move.z;
	}

	// 起伏との判定
	if ((CManager::GetInstance()->GetScene()->GetElevation()->IsHit(pos) || m_bHitStage))
	{// 地面と当たっていたら

		/*m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;*/
		m_KnokBackMove.y = 0.0f;	// 移動量ゼロ
		m_bLandOld = true;
		pos.y = 0.0f;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// ノックバック判定消す
		m_sMotionFrag.bKnockBack = false;

		pMotion->Set(MOTION_KNOCKBACK_PASSIVE);

		// Xファイルとの判定
		CStage* pStage = CGame::GetInstance()->GetStage();
		if (pStage == nullptr)
		{// nullptrだったら
			return;
		}

		// ステージに当たった判定
		for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
		{
			// オブジェクト取得
			CObjectX* pObjX = pStage->GetObj(nCntStage);

			if (pObjX == nullptr)
			{// nullptrだったら
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



	m_pStaminaGauge->SetValue(0.0f);

	if (nType == MOTION_KNOCKBACK_PASSIVE &&
		pMotion->IsFinish())
	{// 受け身もノックバックも終了

		m_pStaminaGauge->SetState(CStaminaGauge_Player::STATE_QUICKHEAL);
		m_state = STATE_DOWN;
		return;
	}
}

//==========================================================================
// ダウン
//==========================================================================
void CPlayer::StateDown()
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}
	pMotion->Set(MOTION_DOWN);

	if (m_pStaminaGauge->GetState() == CStaminaGauge_Player::STATE_NORMAL)
	{
		m_state = STATE_NONE;
	}
}

//==========================================================================
// 死亡状態
//==========================================================================
void CPlayer::StateDead()
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
		if (pStage == nullptr)
		{// nullptrだったら
			return;
		}

		// ステージに当たった判定
		for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
		{
			// オブジェクト取得
			CObjectX *pObjX = pStage->GetObj(nCntStage);

			if (pObjX == nullptr)
			{// nullptrだったら
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
void CPlayer::StateDeadWait()
{
	// ぶっ倒れモーション
	GetMotion()->Set(MOTION_DEAD);
}

//==========================================================================
// フェードアウト状態
//==========================================================================
void CPlayer::StateFadeOut()
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
// リスポーン
//==========================================================================
void CPlayer::StateRespawn()
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
		m_sMotionFrag.bDead = false;
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
void CPlayer::StateCounter()
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
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
void CPlayer::StateAvoid()
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
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
// 祈り
//==========================================================================
void CPlayer::StatePrayer()
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	if (pMotion->IsFinish())
	{// 祈りが終了

		// 祈りループモーション設定
		pMotion->Set(MOTION_PRAYERLOOP);
	}
}

//==========================================================================
// チャージ
//==========================================================================
void CPlayer::StateCharge()
{
	// インプット情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		m_nComboStage = 0;
		return;
	}
	int nType = pMotion->GetType();

	if (!m_bAttacking ||
		(nType != MOTION_ATK4 &&
		nType != MOTION_ATK4_FINISH))
	{
		m_state = STATE_NONE;
		m_nComboStage = 0;
		m_fChargeTime = 0.0f;
		m_bChargeCompletion = false;
	}


	// 状態カウンターループ
	m_nCntState = (m_nCntState + 1) % 4;

	// チャージ時間加算
	if (nType == MOTION_ATK4)
	{
		m_fChargeTime += CManager::GetInstance()->GetDeltaTime();
	}

	if (m_fChargeTime < m_PlayerStatus.chargeTime)
	{
		m_bChargeCompletion = false;
	}

	if (m_fChargeTime >= m_PlayerStatus.chargeTime &&
		!m_bChargeCompletion)
	{// チャージ完了！

		// チャージ完了フラグ
		m_bChargeCompletion = true;

		// 情報取得
		CMotion::Info aInfo = pMotion->GetInfo(pMotion->GetType());

		for (int nCntAttack = 0; nCntAttack < aInfo.nNumAttackInfo; nCntAttack++)
		{
			if (aInfo.AttackInfo[nCntAttack] == nullptr)
			{
				continue;
			}

			// 攻撃情報取得
			CMotion::AttackInfo* AttackInfo = aInfo.AttackInfo[nCntAttack];
			MyLib::Vector3 pos = pMotion->GetAttackPosition(GetModel(), *AttackInfo);

			// チャージ完了
			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_CHARGEFINISH,
				pos,
				0.0f, 0.0f, 40.0f);
		}
	}

	// 塵
	if (nType == MOTION_ATK4 && m_bChargeCompletion && m_nCntState == 0)
	{
		CMyEffekseer::GetInstance()->SetEffect(
			CMyEffekseer::EFKLABEL_CHARGE,
			GetPosition(),
			0.0f, 0.0f, 80.0f);
	}

	// チャージを離してる
	if (!pInputGamepad->GetPress(CInputGamepad::BUTTON_Y, m_nMyPlayerIdx) &&
		nType == MOTION_ATK4 && pMotion->IsFinish())
	{
		pMotion->Set(MOTION_ATK4_FINISH);
		m_fChargeTime = 0.0f;
		m_nComboStage = 0;
		m_state = STATE_NONE;
	}

	if (nType == MOTION_ATK4_FINISH && pMotion->IsFinish())
	{
		m_state = STATE_NONE;
		m_nComboStage = 0;
		m_fChargeTime = 0.0f;
		m_bChargeCompletion = false;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CPlayer::Draw()
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
CPlayer::STATE CPlayer::GetState()
{
	return m_state;
}


//==========================================================================
// 強化リセット
//==========================================================================
void CPlayer::ResetEnhance()
{
	// 体力に設定
	if (m_pHPGauge != nullptr)
	{
		SetLife(m_pHPGauge->UpgradeMaxValue(0));
		SetLifeOrigin(m_pHPGauge->GetMaxLife());
	}

	// スタミナ
	if (m_pStaminaGauge != nullptr)
	{
		m_pStaminaGauge->UpgradeMaxValue(0);
	}

	// スタミナ自動回復値
	if (m_pStaminaGauge != nullptr)
	{
		m_pStaminaGauge->UpgradeAutoHeal(1.0f);
	}

	// 操作関連
	ChangeAtkControl(DEBUG_NEW CPlayerControlAttack());
	ChangeDefenceControl(DEBUG_NEW CPlayerControlDefence());
	ChangeAvoidControl(DEBUG_NEW CPlayerControlAvoid());
	ChangeGuardGrade(DEBUG_NEW CPlayerGuard());

	m_PlayerStatus = sPlayerStatus(
		DEFAULT_RESPAWNHEAL, DEFAULT_SUBVALUE_GUARD, DEFAULT_SUBVALUE_COUNTER,
		DEFAULT_COUNTERHEAL, DEFAULT_MULTIPLY_ATTACK, DEFAULT_CHARGETIME,
		DEFAULT_FRAME_EXTENSION_COUNTER, DEFAULT_MULTIPLY_GUARD, DEFAULT_TIME_ADDDOWN, DEFAULT_IS_CHARGEFLINCH);
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
// スタミナ最大値アップグレード
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

	SetLife(static_cast<int>(static_cast<float>(GetLifeOrigin()) * m_PlayerStatus.respawnHeal));
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
// リスポーン確率加算
//==========================================================================
void CPlayer::AddRespawnPercent(int value)
{
	m_nRespawnPercent += value;
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

//==========================================================================
// 攻撃の終了時設定
//==========================================================================
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