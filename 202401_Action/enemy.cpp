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
#include "tutorial.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "explosion.h"
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
#include "score.h"
#include "bullet.h"
#include "stage.h"
#include "objectX.h"
#include "collisionobject.h"
#include "limitarea.h"
#include "santabag.h"

#include "rockon_marker.h"

// 子クラス
#include "enemy_boss.h"
#include "enemy_cookie.h"
#include "enemy_orafu.h"

//==========================================================================
// 無名名前空間
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
// 静的メンバ変数宣言
//==========================================================================
CListManager<CEnemy> CEnemy::m_List = {};	// リスト


//==========================================================================
// コンストラクタ
//==========================================================================
CEnemy::CEnemy(int nPriority) : CObjectChara(nPriority)
{
	// 値のクリア
	m_posOrigin = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 最初の位置
	memset(&m_sFormationInfo, NULL, sizeof(m_sFormationInfo));	// 隊列の情報
	m_posKnokBack = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// ノックバックの位置
	m_rotOrigin = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 最初の向き
	m_type = TYPE_BOSS;	// 種類
	m_state = STATE_NONE;	// 状態
	m_Oldstate = m_state;	// 前回の状態
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// マテリアルの色
	m_TargetPosition = mylib_const::DEFAULT_VECTOR3;	// 目標の位置
	m_pWeaponHandle = 0;		// エフェクトの武器ハンドル

	m_fStateTime = 0.0f;	// 状態遷移カウンター
	m_nTexIdx = 0;			// テクスチャのインデックス番号
	m_nNumChild = 0;		// この数
	m_nTargetPlayerIndex = 0;	// 追い掛けるプレイヤーのインデックス番号
	m_fActCounter = 0.0f;		// 移動カウンター
	m_bActionable = false;		// 行動可能か
	m_fDownTime = 0.0f;			// ダウンカウンター
	m_fRockOnDistance = 0.0f;	// ロックオンの距離
	m_bAddScore = false;		// スコア加算するかの判定
	m_bRockOnAccepting = false;	// ロックオン受付
	m_nBallastEmission = 0;	// 瓦礫の発生カウンター
	m_sMotionFrag.bJump = false;		// ジャンプ中かどうか
	m_sMotionFrag.bKnockback = false;	// ノックバック中かどうか
	m_sMotionFrag.bMove = false;		// 移動中かどうか
	m_sMotionFrag.bATK = false;			// 攻撃中かどうか
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
CEnemy *CEnemy::Create(const char *pFileName, MyLib::Vector3 pos, TYPE type)
{
	// 生成用のオブジェクト
	CEnemy *pEnemy = nullptr;

	if (pEnemy == nullptr)
	{// nullptrだったら

		// メモリの確保
		switch (type)
		{
		case TYPE_BOSS:
			pEnemy = DEBUG_NEW CEnemyBoss;
			break;

		case TYPE_COOKIE:
			pEnemy = DEBUG_NEW CEnemyCookie;
			break;

		case TYPE_ORAFU:
			pEnemy = DEBUG_NEW CEnemyOrafu;
			break;

		default:
			return nullptr;
			break;
		}

		if (pEnemy != nullptr)
		{// メモリの確保が出来ていたら

			// 種類
			pEnemy->m_type = type;

			// 最初の位置設定
			pEnemy->m_posOrigin = pos;

			// 位置設定
			pEnemy->SetPosition(pos);

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

	return nullptr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemy::Init()
{
	// 各種変数の初期化
	m_state = STATE_NONE;	// 状態
	m_Oldstate = STATE_PLAYERCHASE;
	m_fStateTime = 0.0f;			// 状態遷移カウンター
	m_posKnokBack = m_posOrigin;	// ノックバックの位置
	m_bAddScore = true;	// スコア加算するかの判定
	m_fRockOnDistance = 0.0f;	// ロックオンの距離

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
		/*CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
		if (pPlayer != nullptr)
		{
			pPlayer->SwitchRockOnTarget();
		}*/
		CManager::GetInstance()->GetCamera()->SetRockOn(GetPosition(), false);
	}

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

	if (m_state != STATE_SPAWN &&
		m_state != STATE_SPAWNWAIT)
	{
		// 行動の設定
		ActionSet();
	}

	// 行動更新
	UpdateAction();

	// 種類別処理
	UpdateByType();

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

	// バッグのリスト取得
	CListManager<CSantaBag> BagList = CSantaBag::GetListObj();
	CSantaBag* pBag = nullptr;

	// リストループ
	while (BagList.ListLoop(&pBag))
	{
		m_TargetPosition = pBag->GetPosition();
	}

	// 大人の壁
	LimitArea();

	// ロックオン受付してたら
	if (m_bRockOnAccepting)
	{
		MyLib::Vector3 rockonpos = GetPosition();
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

	if (m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_SPAWN && m_state != STATE_FADEOUT)
	{// なにもない状態の時

		// 当たった
		bHit = true;

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

			/*MyLib::Vector3 move = GetMove();
			move.x = UtilFunc::Transformation::Random(-5, 5) + 20.0f;
			move.y = UtilFunc::Transformation::Random(0, 5) + 15.0f;
			move.z = UtilFunc::Transformation::Random(-5, 5) + 20.0f;
			SetMove(move);*/
			return true;
		}

		// 過去の状態保存
		m_Oldstate = m_state;

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
	m_state = STATE_DMG;

	// 遷移カウンター設定
	m_fStateTime = TIME_DMG;

	// やられモーション
	GetMotion()->Set(MOTION_DMG);

	// ヒットストップ
	CManager::GetInstance()->SetEnableHitStop(5);

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
// 種類別更新処理
//==========================================================================
void CEnemy::UpdateByType()
{

}

//==========================================================================
// 行動更新
//==========================================================================
void CEnemy::UpdateAction()
{

}

//==========================================================================
// ターゲットの方を向く
//==========================================================================
void CEnemy::RotationTarget()
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();

	// 目標の角度を求める
	float fRotDest = atan2f((pos.x - m_TargetPosition.x), (pos.z - m_TargetPosition.z));

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
}

//==========================================================================
// プレイヤーとの距離を判定
//==========================================================================
bool CEnemy::CalcLenPlayer(float fLen)
{
	// プレイヤー情報
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);
	if (pPlayer == nullptr)
	{
		return false;
	}

	return UtilFunc::Collision::CircleRange3D(GetPosition(), pPlayer->GetPosition(), fLen, pPlayer->GetRadius());
}

//==========================================================================
// 移動方向を向く処理
//==========================================================================
void CEnemy::MoveRotation()
{
	// 必要な値を取得
	MyLib::Vector3 rot = GetRotation();
	MyLib::Vector3 move = GetMove();

	// 方向を算出
	float fRot = atan2f(-move.x, -move.z);

	// 角度の正規化
	UtilFunc::Transformation::RotNormalize(fRot);

	// 角度の補正をする
	rot.y += (fRot - rot.y) * 0.025f;

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// 移動
//==========================================================================
void CEnemy::Move()
{
	// 移動フラグを立てる
	m_sMotionFrag.bMove = true;

	// 移動速度取得
	float fMove = GetVelocity();

	// 移動量を適用
	MyLib::Vector3 move = GetMove();
	move.x = sinf(m_fActCounter) * fMove;
	move.z = cosf(m_fActCounter) * fMove;
	SetMove(move);

	// 方向転換
	MoveRotation();
}

//==========================================================================
// 状態更新処理
//==========================================================================
void CEnemy::UpdateState()
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 pos11 = MyLib::Vector3(GetObjectChara()->GetModel()[0]->GetWorldMtx()._41, GetObjectChara()->GetModel()[0]->GetWorldMtx()._42, GetObjectChara()->GetModel()[0]->GetWorldMtx()._43);

	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);

	// ダウンカウンター減算
	m_fDownTime -= CManager::GetInstance()->GetDeltaTime();
	UtilFunc::Transformation::ValueNormalize(m_fDownTime, TIME_DOWN, 0.0f);

	switch (m_state)
	{
	case STATE_NONE:
		StateNone();
		break;

	case STATE_SPAWNWAIT:	// スポーン待機
		SpawnWait();
		break;

	case STATE_SPAWN:
		Spawn();
		break;

	case STATE_DMG:
		Damage();
		break;

	case STATE_DEAD:
		Dead();
		break;

	case STATE_FADEOUT:
		FadeOut();
		break;

	case STATE_PLAYERCHASE:
		PlayerChase();
		break;

	case STATE_PARENTCHASE:
		ParentChase();
		break;

	case STATE_ATTACK:
		StateAttack();
		break;

	case STATE_WAIT:
		StateWait();
		break;

	case STATE_DOWN:
		StateDown();
		break;
	}

	if (m_state != STATE_SPAWNWAIT && IsDisp() == false)
	{
		// 描画する
		SetEnableDisp(true);
	}
}

//==========================================================================
// 種類別状態更新処理
//==========================================================================
void CEnemy::UpdateStateByType()
{
	return;
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
void CEnemy::SpawnWait()
{
	// 行動可能判定
	m_bActionable = false;

	// 状態カウンターリセット
	m_fStateTime = 0.0f;

	// 描画しない
	SetEnableDisp(false);
}

//==========================================================================
// 出現
//==========================================================================
void CEnemy::Spawn()
{
	// 行動可能判定
	m_bActionable = false;

}

//==========================================================================
// ダメージ
//==========================================================================
void CEnemy::Damage()
{
	// 行動可能判定
	m_bActionable = false;

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
// 死亡
//==========================================================================
void CEnemy::Dead()
{
	// 行動可能判定
	m_bActionable = false;

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
		//Uninit();
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
void CEnemy::FadeOut()
{
	// 行動可能判定
	m_bActionable = false;

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

		// スコア加算の判定オフ
		m_bAddScore = false;

		// 敵の終了処理
		Kill();
		Uninit();
		return;
	}
}

//==========================================================================
// プレイヤー追従
//==========================================================================
void CEnemy::PlayerChase()
{
	// 行動可能判定
	m_bActionable = true;

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

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

	// プレイヤー情報
	CPlayer *pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);

	// オブジェクト情報
	CObject *pObj = nullptr;


	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTime <= 0)
	{// 遷移カウンターが0になったら
		m_fStateTime = 0;
	}

	if (pPlayer != nullptr)
	{// nullptrじゃないとき

		// 親の位置取得
		MyLib::Vector3 posPlayer = pPlayer->GetPosition();
		CObject *pMyObj = GetObject();

		// 目標の角度を求める
		fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

		// 目標との差分
		fRotDiff = fRotDest - rot.y;

		//角度の正規化
		UtilFunc::Transformation::RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		UtilFunc::Transformation::RotNormalize(rot.y);

		// 向き設定
		SetRotation(rot);

		// 攻撃状態移行処理
		ChangeToAttackState();

		// 向いてる方向にダッシュ
		if (bLen == false)
		{// 距離が保たれていたら

			// 追い掛け移動処理
			ChaseMove(fMove);
		}

		if (UtilFunc::Collision::CircleRange3D(pos, pPlayer->GetPosition(), 200.0f, PLAYER_SERCH) == false)
		{// プレイヤーが視界から消えたら
			m_state = STATE_NONE;
		}
	}

	// 位置設定
	SetPosition(pos);

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 親追従
//==========================================================================
void CEnemy::ParentChase()
{
	// 行動可能判定
	m_bActionable = true;

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

	// かなり離れてるかの判定
	bool bLongDistance = true;

	// オブジェクト情報
	CObject *pObj = nullptr;


	// 状態遷移カウンター減算
	//m_fStateTime++;

	if (m_fStateTime <= 0)
	{// 遷移カウンターが0になったら
		m_fStateTime = 0;
	}

	if (m_pParent != nullptr)
	{// 親がいる場合

		// 親の移動量取得
		MyLib::Vector3 moveParent = m_pParent->GetMove();

		// 親の位置取得
		MyLib::Vector3 posParent = m_pParent->GetPosition();

		// 親の向き取得
		MyLib::Vector3 rotParent = m_pParent->GetRotation();

		// 目標の位置
		MyLib::Vector3 posDest;
		posDest = posParent;

		// 目標の角度を求める
		fRotDest = atan2f((pos.x - posDest.x), (pos.z - posDest.z));

		// 目標との差分
		fRotDiff = fRotDest - rot.y;

		//角度の正規化
		UtilFunc::Transformation::RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		UtilFunc::Transformation::RotNormalize(rot.y);

		// 向き設定
		SetRotation(rot);

		if (UtilFunc::Collision::CircleRange3D(pos, posDest, 25.0f, CHACE_DISTABCE) == true)
			{// 一定距離間に親が入ったら
				bLen = true;	// 長さ判定
			}

			// 向いてる方向にダッシュ
			if (bLen == false)
			{// 距離が保たれていたら

				// 追い掛け移動処理
				ChaseMove(fMove * 1.4f);
			}

	}
	else
	{// 自分自身が親の時

		float fRotDiff = 0.0f;	// 現在と目標の差分

		// 状態遷移カウンター更新
		m_fStateTime = static_cast<float>((static_cast<int>(m_fStateTime) + 1) % 120);

		// 目標の角度を求める
		int stateTime = static_cast<int>(m_fStateTime);
		if (stateTime == 0)
		{
			fRotDest = UtilFunc::Transformation::Random(-31, 31) * 0.1f;
		}

		// 目標との差分
		fRotDiff = fRotDest - rot.y;

		//角度の正規化
		UtilFunc::Transformation::RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		UtilFunc::Transformation::RotNormalize(rot.y);

		// 向き設定
		SetRotation(rot);

		// 追い掛け移動処理
		ChaseMove(fMove);

#if _DEBUG
		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 0.5f, 1.0f, 1.0f);
#endif
	}

	// プレイヤー追従の判定
	TriggerChasePlayer();

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 攻撃処理
//==========================================================================
void CEnemy::StateAttack()
{
	// 行動可能判定
	m_bActionable = true;

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

	// オブジェクト情報
	CObject *pObj = nullptr;

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}
	int nType = pMotion->GetType();

	if (m_pParent != nullptr)
	{// 親がいる場合

		// 親も追い掛け状態にする
		if (m_pParent->m_state != STATE_ATTACK)
		{
			if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD)
			{
				m_pParent->m_state = STATE_PLAYERCHASE;
				m_pParent->m_fStateTime = 60;
			}

			for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
			{// 子の数分回す
				if (m_pParent->m_pChild[nCntEnemy] == nullptr)
				{
					continue;
				}

				if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD)
				{
					continue;
				}

				m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				m_pParent->m_pChild[nCntEnemy]->m_fStateTime = 60;
			}
		}
	}
	else
	{// 自分が親な場合

		for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
		{// 子の数分回す

			if (m_pChild[nCntEnemy] == nullptr)
			{// nullptrだったら
				continue;
			}

			if (m_pChild[nCntEnemy]->m_state != STATE_ATTACK)
			{// 攻撃状態の時

				if (m_pChild[nCntEnemy]->m_state != STATE_DMG && m_pChild[nCntEnemy]->m_state != STATE_DEAD)
				{
					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
					m_pChild[nCntEnemy]->m_fStateTime = 60;
				}
			}
		}
	}

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		if (nType == 0 && pPlayer != nullptr)
		{// ニュートラルに戻れば

			if (UtilFunc::Collision::CircleRange3D(pos, pPlayer->GetPosition(), 400.0f, PLAYER_SERCH) == false)
			{// プレイヤーと離れすぎていたら

				// 間隔をあける状態にする
				m_state = STATE_NONE;
				break;
			}
			else
			{// まだ追える時

				m_state = STATE_PLAYERCHASE;

				if (m_pParent != nullptr)
				{// 親がいる場合

					// 親も追い掛け状態にする
					if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD)
					{
						m_pParent->m_state = STATE_PLAYERCHASE;
					}

					for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
					{// 子の数分回す

						if (m_pParent->m_pChild[nCntEnemy] == nullptr)
						{
							continue;
						}

						if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD)
						{
							continue;
						}

						m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
					}
				}
				else
				{// 自分が親な場合

					for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
					{// 子の数分回す

						if (m_pChild[nCntEnemy] == nullptr)
						{
							continue;
						}

						if (m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pChild[nCntEnemy]->m_state == STATE_DEAD)
						{
							continue;
						}

						m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
					}
				}
			}
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

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 0.5f, 0.1f, 1.0f);

	if (m_fDownTime <= 0.0f)
	{
		m_state = STATE_NONE;
		// 行動可能判定
		m_bActionable = true;
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
// プレイヤー追従ONにするトリガー
//==========================================================================
void CEnemy::TriggerChasePlayer()
{



	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	int i = 0;
	while (playerList.ListLoop(&pPlayer))
	{
		float fRadius = PLAYER_SERCH;

		if (UtilFunc::Collision::CircleRange3D(pos, pPlayer->GetPosition(), 200.0f, fRadius) == true)
		{// プレイヤーが範囲に入れば
			m_state = STATE_PLAYERCHASE;

			if (m_pParent != nullptr)
			{// 親がいる場合

				// 親も追い掛け状態にする
				if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD)
				{
					m_pParent->m_state = STATE_PLAYERCHASE;
				}

				for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
				{// 子の数分回す

					if (m_pParent->m_pChild[nCntEnemy] == nullptr)
					{
						continue;
					}

					if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD)
					{
						continue;
					}

					// 自分の親の子もプレイヤー追い掛け状態
					m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
			else
			{// 自分が親な場合

				for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
				{// 子の数分回す

					if (m_pChild[nCntEnemy] == nullptr)
					{
						continue;
					}

					if (m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pChild[nCntEnemy]->m_state == STATE_DEAD)
					{
						continue;
					}

					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}

			// 追い掛けるプレイヤーの番号設定
			m_nTargetPlayerIndex = i;
			break;
		}

		i++;	// インデックス加算
	}

}

//==========================================================================
// 攻撃状態移行処理
//==========================================================================
void CEnemy::ChangeToAttackState()
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	int i = 0;
	while (playerList.ListLoop(&pPlayer))
	{
		// 親の位置取得
		MyLib::Vector3 posPlayer = pPlayer->GetPosition();

		if (UtilFunc::Collision::CircleRange3D(pos, posPlayer, 400.0f, pPlayer->GetRadius()) == true && m_sMotionFrag.bJump == false)
		{// 一定距離間にプレイヤーが入ったら

			// 攻撃状態にする
			m_state = STATE_ATTACK;
			m_sMotionFrag.bATK = true;

			// 追い掛けるプレイヤーの番号設定
			m_nTargetPlayerIndex = i;
			break;
		}

		i++;
	}
}

//==========================================================================
// 追い掛け移動
//==========================================================================
void CEnemy::ChaseMove(float fMove)
{
	// 向き取得
	MyLib::Vector3 rot = GetRotation();

	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// 移動量加算
	move.x += sinf(D3DX_PI + rot.y) * fMove;
	move.z += cosf(D3DX_PI + rot.y) * fMove;

	// 移動量設定
	SetMove(move);
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
		// プレイヤーの向き
		MyLib::Vector3 PlayerPos = pPlayer->GetPosition();

		// 判定サイズ取得
		float fRadius = pPlayer->GetRadius();

		if (UtilFunc::Collision::SphereRange(weponpos, PlayerPos, pATKInfo->fRangeSize, fRadius).ishit)
		{// 球の判定

			MyLib::HitResult_Character result = pPlayer->Hit(pATKInfo->nDamage, this, pATKInfo->AtkType);

			if (result.ishit)
			{
				pATKInfo->bEndAtk = true;
			}

			if (result.isdamage)
			{// 当たってたら

				// プレイヤーの向き
				MyLib::Vector3 PlayerRot = pPlayer->GetRotation();

				// ターゲットと敵との向き
				float fRot = atan2f((pos.x - PlayerPos.x), (pos.z - PlayerPos.z));

				// 向きを正面にする
				fRot = D3DX_PI + fRot;
				UtilFunc::Transformation::RotNormalize(fRot);

				// 向き設定
				pPlayer->SetRotation(MyLib::Vector3(PlayerRot.x, fRot, PlayerRot.z));
				pPlayer->SetRotDest(fRot);
				fRot = pPlayer->GetRotation().y;

				// 吹き飛ばし
				pPlayer->SetMove(MyLib::Vector3(
					sinf(fRot) * 4.0f,
					12.0f,
					cosf(fRot) * 4.0f));
			}
		}

		//============================
		// 袋と判定
		//============================
		// バッグのリスト取得
		CListManager<CSantaBag> BagList = CSantaBag::GetListObj();
		CSantaBag* pBag = nullptr;

		// リストループ
		while (BagList.ListLoop(&pBag))
		{
			// 当たり判定
			if (UtilFunc::Collision::CircleRange3D(weponpos, pBag->GetPosition(), pATKInfo->fRangeSize, 80.0f))
			{
				pBag->Hit();
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
// 元の向き
//==========================================================================
void CEnemy::SetOriginRotation(MyLib::Vector3 rot)
{
	m_rotOrigin = rot;
}

//==========================================================================
// スポーン地点設定
//==========================================================================
void CEnemy::SetSpawnPosition(MyLib::Vector3 pos)
{
	m_posOrigin = pos;
}

//==========================================================================
// スポーン地点取得
//==========================================================================
MyLib::Vector3 CEnemy::GetSpawnPosition()
{
	return m_posOrigin;
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
