//=============================================================================
// 
//  プレイヤー処理 [enemy.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "enemy.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "input.h"
#include "calculation.h"
#include "particle.h"
#include "motion.h"
#include "model.h"
#include "elevation.h"
#include "player.h"
#include "camera.h"
#include "ballast.h"
#include "3D_Effect.h"
#include "hp_gauge.h"
#include "shadow.h"
#include "sound.h"
#include "impactwave.h"
#include "stage.h"
#include "objectX.h"
#include "limitarea.h"
#include "debugproc.h"
#include "rockon_marker.h"

// 子クラス
#include "enemy_boss.h"
#include "enemy_gobelin.h"
#include "enemy_stonegolem.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float PLAYER_SERCH = 800.0f;	// プレイヤー探索範囲
	const float CHACE_DISTABCE = 50.0f;	// 追い掛ける時の間隔
	const float TIME_DMG = static_cast<float>(10) / static_cast<float>(mylib_const::DEFAULT_FPS);	// ダメージ時間
	const float TIME_DEAD = static_cast<float>(40) / static_cast<float>(mylib_const::DEFAULT_FPS);	// 死亡時間
	const float TIME_DOWN = static_cast<float>(150) / static_cast<float>(mylib_const::DEFAULT_FPS);	// ダウン時間

}


//==========================================================================
// 関数ポインタ
//==========================================================================
// 状態関数
CEnemy::STATE_FUNC CEnemy::m_StateFunc[] =
{
	&CEnemy::StateNone,		// なし
	&CEnemy::StateSpawnWait,// スポーン待機
	&CEnemy::StateSpawn,	// スポーン
	&CEnemy::StateDamage,	// ダメージ
	&CEnemy::StateDead,		// 死亡
	&CEnemy::StateFadeOut,	// フェードアウト
	&CEnemy::StateWait,		// 待機
	&CEnemy::StateDown,		// ダウン
	&CEnemy::StateStrongAtk,// 強攻撃
};

// 行動関数
CEnemy::ACT_FUNC CEnemy::m_ActFuncList[] =
{
	&CEnemy::ActDefault,			// 通常行動
	&CEnemy::ActWait,				// 待機行動
};

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CListManager<CEnemy> CEnemy::m_List = {};	// リスト

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemy::CEnemy(int nPriority) : CObjectChara(nPriority)
{
	// 値のクリア
	m_posKnokBack = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// ノックバックの位置
	m_type = TYPE_BOSS;	// 種類
	m_state = STATE_NONE;	// 状態
	m_Oldstate = m_state;	// 前回の状態
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// マテリアルの色
	m_TargetPosition = mylib_const::DEFAULT_VECTOR3;	// 目標の位置
	m_pWeaponHandle = 0;		// エフェクトの武器ハンドル

	m_Action = ACTION_DEF;		// 行動
	m_fActTime = 0.0f;			// 行動カウンター
	m_fStrongAttackTime = 0.0f;	// 強攻撃のタイマー
	m_fStateTime = 0.0f;		// 状態遷移カウンター
	m_nNumChild = 0;			// 子の数
	m_nTargetPlayerIndex = 0;	// 追い掛けるプレイヤーのインデックス番号
	m_bDamageReceived = false;				// ダメージ受け付け判定
	m_fDamageReciveTime = 0.0f;				// ダメージ受付時間
	m_bActiveSuperArmor = false;			// スーパーアーマー
	m_bActionable = false;		// 行動可能か
	m_fDownTime = 0.0f;			// ダウンカウンター
	m_fRockOnDistance = 0.0f;	// ロックオンの距離
	m_bRockOnAccepting = false;	// ロックオン受付

	m_bStateChanging = false;	// 状態が切り替わった瞬間
	m_pATKState = nullptr;		// 今の行動ポインタ
	m_pNextATKState = nullptr;	// 次の行動ポインタ
	m_bCatchUp = false;						// 追い着き判定
	m_bInSight = false;						// 視界内判定
	m_sMotionFrag = SMotionFrag();		// モーションのフラグ
	m_pParent = nullptr;					// 親のポインタ
	m_pHPGauge = nullptr;					// HPゲージの情報
	m_pShadow = nullptr;
	m_pRockOnMarker = nullptr;		// ロックオンマーカー


	memset(&m_pChild[0], NULL, sizeof(m_pChild));	// 子のポインタ
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemy::~CEnemy()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEnemy* CEnemy::Create(const char* pFileName, MyLib::Vector3 pos, TYPE type)
{
	// 生成用のオブジェクト
	CEnemy* pEnemy = nullptr;

	// メモリの確保
	switch (type)
	{
	case TYPE_BOSS:
		pEnemy = DEBUG_NEW CEnemyBoss;
		break;

	case TYPE_STONEGOLEM:
		pEnemy = DEBUG_NEW CEnemyGolem;
		break;

	case TYPE_GOBELIN:
		pEnemy = DEBUG_NEW CEnemyGobelin;
		break;

	default:
		return nullptr;
		break;
	}

	if (pEnemy != nullptr)
	{// メモリの確保が出来ていたら

		// 種類
		pEnemy->m_type = type;

		// 位置設定
		pEnemy->SetPosition(pos);
		pEnemy->SetOriginPosition(pos);

		// テキスト読み込み
		HRESULT hr = pEnemy->RoadText(pFileName);
		if (FAILED(hr))
		{// 失敗していたら
			return nullptr;
		}

		// 初期化処理
		pEnemy->Init();
	}

	return pEnemy;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemy::Init()
{
	// 各種変数の初期化
	m_state = STATE_NONE;	// 状態
	m_Oldstate = STATE_NONE;
	m_fStateTime = 0.0f;			// 状態遷移カウンター
	m_posKnokBack = GetOriginPosition();	// ノックバックの位置
	m_fRockOnDistance = 0.0f;	// ロックオンの距離
	m_bDamageReceived = true;	// ダメージ受付フラグ

	// 種類の設定
	SetType(TYPE_ENEMY);

	// 向き設定
	SetRotation(MyLib::Vector3(0.0f, 0.0f, 0.0f));

	// リストに追加
	m_List.Regist(this);

	return S_OK;
}

//==========================================================================
// テキスト読み込み
//==========================================================================
HRESULT CEnemy::RoadText(const char *pFileName)
{
	// キャラ作成
	HRESULT hr = SetCharacter(pFileName);
	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 子登録
//==========================================================================
void CEnemy::RegistrChild(CEnemy *pChild)
{
	if (m_nNumChild >= mylib_const::MAX_ENEMY)
	{
		return;
	}

	if (m_pChild[m_nNumChild] == nullptr)
	{
		m_pChild[m_nNumChild] = pChild;
		m_nNumChild++;	// 子の数加算
	}
	else
	{
		int n = 0;
	}
}

//==========================================================================
// 子のリセット
//==========================================================================
void CEnemy::ResetChild(CEnemy *pChild)
{
	int nMax = m_nNumChild;

	for (int nCntChild = 0; nCntChild < nMax; nCntChild++)
	{
		if (m_pChild[nCntChild] == nullptr)
		{
			continue;
		}

		if (m_pChild[nCntChild] == pChild)
		{
			m_pChild[nCntChild] = nullptr;
		}
	}
}


//==========================================================================
// 親のポインタ設定
//==========================================================================
void CEnemy::SetParent(CEnemy *pParent)
{
	if (this != nullptr)
	{
		// 子の割り当て
		if (pParent != nullptr)
		{
			pParent->RegistrChild(this);
		}

		// 親のポインタ渡す
		m_pParent = pParent;
		
	}
	else
	{
		int n = 0;
	}
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemy::Uninit()
{
	
	// 影を消す
	if (m_pShadow != nullptr)
	{
		m_pShadow = nullptr;
	}

	if (m_pRockOnMarker != nullptr)
	{
		m_pRockOnMarker = nullptr;
	}

	// リストから削除
	m_List.Delete(this);

	// 終了処理
	CObjectChara::Uninit();
}

//==========================================================================
// 死亡処理
//==========================================================================
void CEnemy::Kill()
{
	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_ENEMY; nCntEnemy++)
	{// 子の数分回す

		if (m_pChild[nCntEnemy] != nullptr)
		{// 子がいる場合

			// 自分の子の親をnullptrにする
			m_pChild[nCntEnemy]->m_pParent = nullptr;
		}
	}

	if (m_pParent != nullptr)
	{// 自分に親がいる場合

		// 親の子をnullptrにする
		m_pParent->ResetChild(this);
	}

	// HPゲージを消す
	if (m_pHPGauge != nullptr)
	{
		m_pHPGauge->Uninit();
		m_pHPGauge = nullptr;
	}

	// 影を消す
	if (m_pShadow != nullptr)
	{
		m_pShadow->Uninit();
		m_pShadow = nullptr;
	}

	// ロックオンマーカー
	if (m_pRockOnMarker != nullptr)
	{
		m_pRockOnMarker->Kill();
		m_pRockOnMarker = nullptr;
	}

	// ロックオン受付してたら
	if (m_bRockOnAccepting)
	{
		CManager::GetInstance()->GetCamera()->SetRockOn(GetPosition(), false);
	}

}


//==========================================================================
// 攻撃状態切り替え
//==========================================================================
void CEnemy::ChangeATKState(CEnemyState* state)
{

	// 状態が切り替わった瞬間フラグ
	m_bStateChanging = true;

	if (m_pATKState != nullptr &&
		!m_pATKState->IsCreateFirstTime())
	{
		delete m_pATKState;
	}
	m_pATKState = state;

	m_bCatchUp = false;
	m_bInSight = false;
}

//==========================================================================
// 攻撃処理
//==========================================================================
void CEnemy::PerformAttack()
{
	if (m_pATKState == nullptr)
	{
		return;
	}

	if (m_bActionable)
	{
		m_pATKState->Action(this);
	}
}

//==========================================================================
// 攻撃状態切り替え
//==========================================================================
void CEnemy::DrawingRandomAction()
{
	// ランダムにアクションパターンを選択して実行
	if (!m_pAtkPattern.empty())
	{
		if (m_state == STATE_STRONGATK)
		{
			m_state = STATE_NONE;
		}

		int randomIndex = rand() % m_pAtkPattern.size();

		if (!m_pAtkPattern[randomIndex]->IsDirectlyTrans())
		{// 何か挟んで行動する場合

			// 次の行動設定
			ChangeNextATKState(m_pAtkPattern[randomIndex]);

			// 遷移前処理
			m_pAtkPattern[randomIndex]->BeforeTransitionProcess(this);
		}
		else
		{
			ChangeATKState(m_pAtkPattern[randomIndex]);
			m_bCatchUp = false;
			m_bInSight = false;

			// モーションインデックス切り替え
			m_pATKState->ChangeMotionIdx(this);
		}
	}
}

//==========================================================================
// 次の攻撃へ切り替え
//==========================================================================
void CEnemy::ChangeNextAction()
{
	// 保存していた次の行動を設定
	ChangeATKState(m_pNextATKState);
	m_bCatchUp = false;
	m_bInSight = false;

	// モーションインデックス切り替え
	m_pATKState->ChangeMotionIdx(this);
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemy::Update()
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 状態が切り替わった瞬間フラグ
	m_bStateChanging = false;

	// エディット中は抜ける
	if (CGame::GetInstance()->GetElevation()->IsEdit())
	{
		return;
	}

	if (!CGame::GetInstance()->GetGameManager()->IsControll())
	{// 行動できるとき
		return;
	}

	// 親の処理
	CObjectChara::Update();

	// 過去の位置設定
	SetOldPosition(GetPosition());


	// プレイヤー情報
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);
	if (pPlayer == nullptr)
	{
		return;
	}
	m_TargetPosition = pPlayer->GetPosition();


	// 行動別処理
	(this->*(m_ActFuncList[m_Action]))();

	// 当たり判定
	Collision();

	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// モーションの設定処理
	MotionSet();

	// 状態更新
	UpdateState();

	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// HPゲージの位置更新
	if (m_pHPGauge != nullptr)
	{
		// 位置取得
		MyLib::Vector3 pos = GetPosition();

		// 体力取得
		int nLife = GetLife();

		m_pHPGauge->UpdatePosition(pos, nLife);
	}

	// 影の位置更新
	if (m_pShadow != nullptr)
	{
		// 位置取得
		MyLib::Vector3 pos = GetPosition();
		m_pShadow->SetPosition(MyLib::Vector3(pos.x, m_pShadow->GetPosition().y, pos.z));
	}

	if (GetPosition().y <= mylib_const::KILL_Y)
	{
		// 敵の終了処理
		Kill();
		Uninit();
		return;
	}

	// 大人の壁
	LimitArea();

	// ロックオン受付してたら
	if (m_bRockOnAccepting)
	{
		MyLib::Vector3 rockonpos = GetPosition();
		MyLib::Vector3 centerpos = GetCenterPosition();
		rockonpos.x = centerpos.x;
		rockonpos.z = centerpos.z;
		rockonpos.y += GetHeight() * 0.5f;
		CManager::GetInstance()->GetCamera()->SetRockOn(rockonpos, true);
		CManager::GetInstance()->GetCamera()->SetRockOnDistance(m_fRockOnDistance);

		// ロックオンマーカーの位置設定
		if (m_pRockOnMarker == nullptr)
		{
			m_pRockOnMarker = CRockOnMarker::Create(rockonpos);
		}
		m_pRockOnMarker->SetPosition(rockonpos);
	}
	else
	{
		if (m_pRockOnMarker != nullptr)
		{
			m_pRockOnMarker->Kill();
			m_pRockOnMarker = nullptr;
		}
	}
}

//==========================================================================
// 当たり判定
//==========================================================================
void CEnemy::Collision()
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// 向き取得
	MyLib::Vector3 rot = GetRotation();

	// 重力処理
	move.y -= mylib_const::GRAVITY;

	// 位置更新
	pos += move;

	// 慣性補正
	if (m_state != STATE_SPAWN && m_state != STATE_DMG && m_state != STATE_DEAD)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	if (move.x >= 0.1f || move.x <= -0.1f ||
		move.z >= 0.1f || move.z <= -0.1f)
	{// 移動中
		m_sMotionFrag.bMove = true;
	}
	else
	{
		m_sMotionFrag.bMove = false;
	}

	// 高さ取得
	bool bLand = false;
	float fHeight = CGame::GetInstance()->GetElevation()->GetHeight(pos, &bLand);

	if (fHeight > pos.y)
	{// 地面の方が自分より高かったら

		// 地面の高さに補正
		if (m_state != STATE_DEAD && m_state != STATE_FADEOUT)
		{
			pos.y = fHeight;
		}

		if (bLand == true)
		{// 着地してたら

			// 着地時処理
			ProcessLanding();

			// ジャンプ使用可能にする
			if (m_state != STATE_DMG && m_state != STATE_DEAD)
			{
				move.y = 0.0f;
			}
			m_sMotionFrag.bJump = false;
		}
	}


	if (pos.y <= -800.0f)
	{
		pos.y = -800.0f;
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// 着地時の処理
//==========================================================================
void CEnemy::ProcessLanding()
{
	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// ジャンプ使用可能にする
	if (m_state != STATE_DMG && m_state != STATE_DEAD)
	{
		move.y = 0.0f;
	}
	m_sMotionFrag.bJump = false;

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// ヒット処理
//==========================================================================
bool CEnemy::Hit(const int nValue, CGameManager::AttackType atkType)
{
	bool bHit = false;

	// 向き取得
	MyLib::Vector3 rot = GetRotation();
	MyLib::Vector3 pos = GetPosition();

	// 体力取得
	int nLife = GetLife();

	if (m_bDamageReceived)
	{// ダメージ受付中のみ

		// 当たった
		bHit = true;

		// ダメージ受付フラグリセット
		m_bDamageReceived = false;

		// 体力減らす
		nLife -= nValue;
		UtilFunc::Transformation::ValueNormalize(nLife, GetLifeOrigin(), 0);

		// 体力設定
		SetLife(nLife);

		if (nLife > 0)
		{
			// ダメージ音再生
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_DMG01);
		}
		else
		{// 体力がなくなったら

			// ヒットストップ
			CManager::GetInstance()->SetEnableHitStop(2);

			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(5, 10.0f, 0.0f);

			// 死亡状態にする
			m_state = STATE_DEAD;

			// 遷移カウンター設定
			m_fStateTime = TIME_DEAD;

			// ノックバックの位置更新
			m_posKnokBack = GetPosition();

			// ノックバック判定にする
			m_sMotionFrag.bKnockback = true;

			// やられモーション
			//pMotion->Set(MOTION_KNOCKBACK);

			// 爆発再生
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_ENEMYEXPLOSION);

			return true;
		}

		// 過去の状態保存
		m_Oldstate = m_state;

		// ダメージ時間設定
		m_fDamageReciveTime = TIME_DMG;

		switch (atkType)
		{
		case CGameManager::ATTACK_NORMAL:
			NormalHitResponse();
			break;

		case CGameManager::ATTACK_STRONG:
			break;

		case CGameManager::ATTACK_COUNTER:
			CounterHitResponse();
			break;

		default:
			break;
		}
	}

	return bHit;
}

//==========================================================================
// 通常ヒット時の反応
//==========================================================================
void CEnemy::NormalHitResponse()
{
	if (m_pHPGauge == nullptr)
	{
		// 体力ゲージ
		m_pHPGauge = CHP_Gauge::Create(GetHeight(), GetLifeOrigin(), 3.0f);
	}

	// ダメージ状態にする
	if (m_state != STATE_DOWN &&
		m_state != STATE_STRONGATK)
	{
		m_state = STATE_DMG;

		// やられモーション
		GetMotion()->Set(MOTION_DMG);
	}

	// 遷移カウンター設定
	m_fStateTime = TIME_DMG;

	// ヒットストップ
	//CManager::GetInstance()->SetEnableHitStop(5);

	// 振動
	CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);

	// ノックバックの位置更新
	m_posKnokBack = GetPosition();
}

//==========================================================================
// カウンターヒット時の反応
//==========================================================================
void CEnemy::CounterHitResponse()
{
	if (m_pHPGauge == nullptr)
	{
		// 体力ゲージ
		m_pHPGauge = CHP_Gauge::Create(GetHeight(), GetLifeOrigin(), 3.0f);
	}

	// ダウン状態にする
	m_state = STATE_DOWN;

	// 遷移カウンター設定
	m_fStateTime = TIME_DMG;

	// ダウンカウンター設定
	m_fDownTime = TIME_DOWN;

	// やられモーション
	GetMotion()->Set(MOTION_DOWN);

	// ヒットストップ
	CManager::GetInstance()->SetEnableHitStop(5);

	// 振動
	CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);

	// ノックバックの位置更新
	m_posKnokBack = GetPosition();
}

//==========================================================================
// ダメージ受付時間更新
//==========================================================================
void CEnemy::UpdateDamageReciveTimer()
{
	// ダメージ受け付け時間減算
	m_fDamageReciveTime -= CManager::GetInstance()->GetDeltaTime();
	if (m_fDamageReciveTime <= 0.0f)
	{
		// ダメージ受け付け判定
		m_bDamageReceived = true;
		m_fDamageReciveTime = 0.0f;
	}
}

//==========================================================================
// 通常行動
//==========================================================================
void CEnemy::ActDefault()
{
	// 攻撃実行処理
	PerformAttack();
}

//==========================================================================
// 待機行動
//==========================================================================
void CEnemy::ActWait()
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	// 待機モーション設定
	pMotion->Set(MOTION_DEF);

	// ターゲットの方を向く
	RotationTarget();

	// 行動カウンター加算
	m_fActTime += CManager::GetInstance()->GetDeltaTime();
}

//==========================================================================
// 追い掛け
//==========================================================================
void CEnemy::ActChase(float moveMultiply, float catchLen)
{
	// 移動モーション
	GetMotion()->Set(MOTION_WALK);

	// 移動フラグを立てる
	m_sMotionFrag.bMove = true;

	// 情報取得
	MyLib::Vector3 move = GetMove();
	MyLib::Vector3 rot = GetRotation();
	float fMove = GetVelocity();

	// 移動量設定
	move.x += sinf(D3DX_PI + rot.y) * fMove * moveMultiply;
	move.z += cosf(D3DX_PI + rot.y) * fMove * moveMultiply;

	// 移動量設定
	SetMove(move);

	// 追い着き判定
	m_bCatchUp = UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, catchLen, 0.0f);
}

//==========================================================================
// ターゲットの方を向く
//==========================================================================
void CEnemy::RotationTarget(float range)
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();

	// 目標の角度を求める
	float fRotDest = pos.AngleXZ(m_TargetPosition);

	// 目標との差分
	float fRotDiff = fRotDest - rot.y;

	//角度の正規化
	UtilFunc::Transformation::RotNormalize(fRotDiff);

	//角度の補正をする
	rot.y += fRotDiff * 0.1f;
	UtilFunc::Transformation::RotNormalize(rot.y);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);


	// 視界判定
	m_bInSight = UtilFunc::Collision::CollisionViewRange3D(GetPosition(), m_TargetPosition, rot.y, range);
}

//==========================================================================
// 状態更新処理
//==========================================================================
void CEnemy::UpdateState()
{
	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);

	// ダメージ受付時間更新
	UpdateDamageReciveTimer();

	// ダウンカウンター減算
	m_fDownTime -= CManager::GetInstance()->GetDeltaTime();
	if (m_fDownTime <= 0.0f)
	{
		m_fDownTime = 0.0f;
	}

	// 状態更新
	(this->*(m_StateFunc[m_state]))();

	if (m_state != STATE_SPAWNWAIT && IsDisp() == false)
	{
		// 描画する
		SetEnableDisp(true);
	}
}

//==========================================================================
// 何もない状態
//==========================================================================
void CEnemy::StateNone()
{
	// 行動可能判定
	m_bActionable = true;

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTime <= 0)
	{// 遷移カウンターが0になったら
		m_fStateTime = 0;
	}
}

//==========================================================================
// スポーン待機
//==========================================================================
void CEnemy::StateSpawnWait()
{
	// 行動可能判定
	m_bActionable = false;

	// ダメージを受け付けない
	m_bDamageReceived = false;

	// 状態カウンターリセット
	m_fStateTime = 0.0f;

	// 描画しない
	SetEnableDisp(false);
}

//==========================================================================
// 出現
//==========================================================================
void CEnemy::StateSpawn()
{
	// 行動可能判定
	m_bActionable = false;

}

//==========================================================================
// ダメージ
//==========================================================================
void CEnemy::StateDamage()
{
	if (!m_bActiveSuperArmor)
	{
		// 行動可能判定
		m_bActionable = false;
	}

	// ダメージを受け付けない
	m_bDamageReceived = false;

#if _DEBUG
	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
#endif

	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTime <= 0.0f)
	{
		// 過去の状態にする
		m_state = m_Oldstate;
	}

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	if (!m_bActiveSuperArmor)
	{
		// ダメージモーション設定
		pMotion->Set(MOTION_DMG);
	}
}

//==========================================================================
// 死亡
//==========================================================================
void CEnemy::StateDead()
{
	// 行動可能判定
	m_bActionable = false;

	// ダメージを受け付けない
	m_bDamageReceived = false;

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
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);
	m_mMatcol.a -= 1.0f / 80.0f;

	// 重力で落下
	move.y += -mylib_const::GRAVITY * 0.25f;
	pos.y += move.y;

	// 回転
	rot.y += D3DX_PI * 0.025f;
	rot.x += D3DX_PI * (UtilFunc::Transformation::Random(5, 25) * 0.001f);

	if(CGame::GetInstance()->GetElevation()->IsHit(pos))
	{
		// パーティクル生成
		my_particle::Create(pos, my_particle::TYPE_ENEMY_FADE);

		// 敵の終了処理
		m_state = STATE_FADEOUT;
		return;
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
// フェードアウト
//==========================================================================
void CEnemy::StateFadeOut()
{
	// 行動可能判定
	m_bActionable = false;

	// ダメージを受け付けない
	m_bDamageReceived = false;

	// 移動量取得
	float fMove = GetVelocity();

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	// フェードアウトモーション設定
	pMotion->Set(MOTION_FADEOUT);

	// フェードアウトのフレーム数
	int nAllFrame = pMotion->GetMaxAllCount(MOTION_FADEOUT);
	float fFrame = pMotion->GetFrameCount();

	// モーションの情報取得
	CMotion::Info aInfo = pMotion->GetInfo(pMotion->GetType());

	// 攻撃情報の総数取得
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	// 高さ取得
	bool bLand = false;
	float fHeight = CGame::GetInstance()->GetElevation()->GetHeight(pos, &bLand);

	m_sMotionFrag.bMove = false;	// 移動判定OFF
	m_sMotionFrag.bATK = false;		// 攻撃判定OFF

	// 遷移カウンター加算
	m_fStateTime++;

	// 色設定
	m_mMatcol.a = 1.0f - ((float)m_fStateTime / (float)nAllFrame);

	if (m_fStateTime >= nAllFrame)
	{// 遷移カウンターがモーションを超えたら

		// 敵の終了処理
		Kill();
		Uninit();
		return;
	}
}

//==========================================================================
// 待機状態
//==========================================================================
void CEnemy::StateWait()
{
	// 行動可能判定
	m_bActionable = true;

	return;
}

//==========================================================================
// ダウン状態
//==========================================================================
void CEnemy::StateDown()
{
	// 行動可能判定
	m_bActionable = false;

	if (m_fDownTime <= 0.0f)
	{
		m_state = STATE_NONE;
		// 行動可能判定
		m_bActionable = true;

		// 行動抽選
		DrawingRandomAction();
		return;
	}

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType != MOTION_DOWN)
	{
		// ダウンモーション設定
		pMotion->Set(MOTION_DOWN);
	}
}

//==========================================================================
// 強攻撃
//==========================================================================
void CEnemy::StateStrongAtk()
{
	// 行動可能判定
	m_bActionable = true;
}

//==========================================================================
//  大人の壁
//==========================================================================
void CEnemy::LimitArea()
{
	// 自身の値を取得
	MyLib::Vector3 pos = GetPosition();

	// 大人の壁取得
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

	// 値を適用
	SetPosition(pos);


	float fLength = sqrtf(pos.x * pos.x + pos.z * pos.z);

	if (fLength > mylib_const::RADIUS_STAGE)
	{// 補正
		D3DXVec3Normalize(&pos, &pos);

		pos *= mylib_const::RADIUS_STAGE;

		SetPosition(pos);
	}
}

//==========================================================================
// モーションの設定
//==========================================================================
void CEnemy::SetMotion(int motionIdx)
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
// 攻撃時処理
//==========================================================================
void CEnemy::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	return;
}

//==========================================================================
// 攻撃判定中処理
//==========================================================================
void CEnemy::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
{
	//return;
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// 武器の位置
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), *pATKInfo);

	if (pATKInfo->fRangeSize == 0.0f)
	{
		return;
	}

	if (pATKInfo->bEndAtk)
	{
		return;
	}

#if _DEBUG
	CEffect3D::Create(weponpos, MyLib::Vector3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), pATKInfo->fRangeSize, 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
#endif

	//============================
	// プレイヤーと判定
	//============================
	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		// コライダーの数繰り返し
		std::vector<SphereCollider> colliders = pPlayer->GetSphereColliders();
		for (const auto& collider : colliders)
		{
			MyLib::HitResult hitresult = UtilFunc::Collision::SphereRange(weponpos, collider.center, pATKInfo->fRangeSize, collider.radius);
			if (hitresult.ishit)
			{// 球の判定

				MyLib::HitResult_Character result = pPlayer->Hit(pATKInfo->nDamage, this, pATKInfo->AtkType);

				if (result.ishit || result.isdeath)
				{
					pATKInfo->bEndAtk = true;
				}

				if (result.isdamage)
				{// 当たってたら

					// プレイヤーの向き
					MyLib::Vector3 PlayerRot = pPlayer->GetRotation();

					// プレイヤーの位置
					MyLib::Vector3 PlayerPos = pPlayer->GetPosition();

					// ターゲットと敵との向き
					float fRot = atan2f((pos.x - PlayerPos.x), (pos.z - PlayerPos.z));

					// 向きを正面にする
					fRot = D3DX_PI + fRot;
					UtilFunc::Transformation::RotNormalize(fRot);

					// 向き設定
					pPlayer->SetRotation(MyLib::Vector3(PlayerRot.x, fRot, PlayerRot.z));
					pPlayer->SetRotDest(fRot);
				}
				break;
			}
		}
	}

}

//==========================================================================
// 描画処理
//==========================================================================
void CEnemy::Draw()
{
	if (m_state == STATE_FADEOUT)
	{
		CObjectChara::Draw(m_mMatcol.a);
	}
	else if (m_mMatcol != D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a))
	{
		// オブジェクトキャラの描画
		CObjectChara::Draw(m_mMatcol);
	}
	else
	{
		// オブジェクトキャラの描画
		CObjectChara::Draw();
	}
}

//==========================================================================
// 状態設定
//==========================================================================
void CEnemy::SetState(STATE state)
{
	m_state = state;
}

//==========================================================================
// 敵の情報取得
//==========================================================================
CEnemy *CEnemy::GetEnemy()
{
	// 自分自身のポインタを取得
	return this;
}



//==========================================================================
// ステップの行動
//==========================================================================
void CEnemyBeforeAction::Action(CEnemy* boss)
{
	// モーション取得
	CMotion* pMotion = boss->GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType == m_nIdxMotion && pMotion->IsFinish() == true)
	{// ステップ終了

		// 次の行動設定
		boss->ChangeNextAction();

		// 待機モーション設定
		pMotion->Set(CEnemy::MOTION_DEF);
		return;
	}

	if (nType != m_nIdxMotion)
	{
		// モーション設定
		pMotion->Set(m_nIdxMotion);
	}
}


//==========================================================================
// 攻撃処理
//==========================================================================
void CEnemyAttack::Attack(CEnemy* boss)
{
	// 判定毎の向き合わせ
	if ((m_bSetAngleBeforeAttack && boss->GetMotion()->IsBeforeInAttack()) ||
		(m_bSetAngleNotAttacking && !boss->GetMotion()->IsAttacking()) &&
		!boss->GetMotion()->IsAllAfterAttack())
	{
		// ターゲットの方を向く
		boss->RotationTarget();
	}

	// モーション取得
	CMotion* pMotion = boss->GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}
	int nType = pMotion->GetType();

	// 連続攻撃する場合
	if (m_bChainAttack &&
		nType == m_nIdxMotion &&
		pMotion->IsGetCombiable())
	{
		// 連続攻撃へ遷移
		ChangeChainAttack(boss);
		return;
	}

	if (nType == m_nIdxMotion && pMotion->IsFinish() == true)
	{// 攻撃が終わってたら

		// 次の行動抽選
		boss->DrawingRandomAction();

		// 待機モーション設定
		pMotion->Set(CEnemy::MOTION_DEF);

		return;
	}

	if (nType != m_nIdxMotion)
	{
		// モーション設定
		pMotion->Set(m_nIdxMotion);
	}
}


//==========================================================================
// 近接攻撃の行動
//==========================================================================
void CEnemyProximity::Action(CEnemy* boss)
{
	bool bAct = true;	// 行動できるかのフラグ

	// 視界内判定
	if (!boss->IsCatchUp() ||
		!boss->IsInSight())
	{
		// ターゲットの方を向く
		boss->RotationTarget();
		bAct = false;
	}

	// 範囲外時追従
	if (!boss->IsCatchUp())
	{
		// 追い掛け
		boss->ActChase(1.0f, m_fAttackLength);
		bAct = false;
	}

	if (!bAct)
	{
		return;
	}

	// 攻撃前行動
	if (!m_bBeforeAttackAction)
	{
		BeforeAttack(boss);
	}

	// 攻撃処理
	if (m_bBeforeAttackAction)
	{
		Attack(boss);
	}
}

//==========================================================================
// 遠距離攻撃の行動
//==========================================================================
void CEnemyRemote::Action(CEnemy* boss)
{
	bool bAct = true;	// 行動できるかのフラグ

	// 視界内判定
	// ターゲットの方を向く
	boss->RotationTarget(45.0f);
	if (!boss->IsInSight())
	{
		// ターゲットの方を向く
		boss->RotationTarget(45.0f);
		bAct = false;
	}

	if (!bAct)
	{
		return;
	}
	// 攻撃処理
	Attack(boss);
}
