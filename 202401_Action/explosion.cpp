//=============================================================================
// 
//  爆発処理 [explosion.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "explosion.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "explosion.h"
#include "3D_effect.h"
#include "sound.h"
#include "particle.h"
#include "enemy.h"
#include "calculation.h"
#include "game.h"
#include "player.h"
#include "impactwave.h"

//==========================================================================
//  定数定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\explosion.png";
	const int TIME_ADDSIZE = 20;		// サイズ加算時間
	const int TIME_FADEOUT = 40;		// フェードアウト時間
	const int MAX_LIFE = 60 * 2;		// 最大寿命
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CExplosion::STATE_FUNC CExplosion::m_FuncList[] =
{
	&CExplosion::StateNone,		// なにもない
	&CExplosion::StateDamage,	// ダメージ
	&CExplosion::StateFadeout,	// フェードアウト
};

CExplosion::COLLISION_FUNC CExplosion::m_CollisionFuncList[] =	// 当たり判定のリスト
{
	&CExplosion::CollisionEnemy,	// 敵との判定
	&CExplosion::CollisionPlayer,	// プレイヤーとの判定
};

//==========================================================================
// コンストラクタ
//==========================================================================
CExplosion::CExplosion(int nPriority) : CMeshSphere(nPriority)
{
	// 値のクリア
	m_type = TYPE_PLAYER;	// 爆発の種類
	m_state = STATE_NONE;	// 状態
	m_nCntState = 0;		// 状態遷移カウンター
	m_nLife = 0;			// 寿命
	m_nLifeMax = 0;			// 寿命の最大値
	m_nTexIdx = 0;			// テクスチャのインデックス番号
	m_nCntEmission = 0;		// 発生物のカウンター
	m_fDestSize = 0.0f;		// 目標のサイズ
}

//==========================================================================
// デストラクタ
//==========================================================================
CExplosion::~CExplosion()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CExplosion *CExplosion::Create(TYPE type, const MyLib::Vector3 pos, const float fSize)
{
	// 生成用のオブジェクト
	CExplosion *pBullet = nullptr;

	if (pBullet == nullptr)
	{// nullptrだったら

		// メモリの確保
		pBullet = DEBUG_NEW CExplosion;

		if (pBullet != nullptr)
		{// メモリの確保が出来ていたら

			// 種類設定
			pBullet->m_type = type;

			// 位置割り当て
			pBullet->SetPosition(pos);

			// サイズ設定
			pBullet->SetWidthLen(0.0f);
			pBullet->SetHeightLen(0.0f);
			pBullet->SetWidthBlock(16);
			pBullet->SetHeightBlock(16);
			pBullet->m_fDestSize = fSize;	// 目標のサイズ

			// 初期化処理
			pBullet->Init();
		}

		return pBullet;
	}

	return nullptr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CExplosion::Init()
{
	HRESULT hr;

	// 各種変数の初期化
	m_nLifeMax = MAX_LIFE;
	m_nLife = m_nLifeMax;	// 寿命
	SetColor(D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f));

	// テクスチャの割り当て
	m_nTexIdx = CTexture::GetInstance()->Regist(TEXTURE);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 種類の設定
	CObject::SetType(TYPE_OBJECTMESH);

	// 初期化処理
	hr = CMeshSphere::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CExplosion::Uninit()
{
	// 終了処理
	CMeshSphere::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CExplosion::Update()
{
	// 位置更新
	UpdatePos();

	// 状態別処理
	(this->*(m_FuncList[m_state]))();

	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 寿命減算
	m_nLife--;

	if (m_nLife <= 0)
	{// 寿命が尽きたら
		m_state = STATE_FADEOUT;
	}

	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 移動処理
//==========================================================================
void CExplosion::UpdatePos()
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// 向き取得
	MyLib::Vector3 rot = GetRotation();

	// 位置更新
	pos += move;

	// 回転
	rot.y += D3DX_PI * 0.025f;
	UtilFunc::Transformation::RotNormalize(rot.y);

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// 通常状態処理
//==========================================================================
void CExplosion::StateNone()
{
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	fWidthLen = UtilFunc::Correction::EasingEaseInOut(0.0f, m_fDestSize, static_cast<float>((m_nLifeMax - m_nLife)) / static_cast<float>(TIME_ADDSIZE));
	fHeightLen = UtilFunc::Correction::EasingEaseInOut(0.0f, m_fDestSize, static_cast<float>((m_nLifeMax - m_nLife)) / static_cast<float>(TIME_ADDSIZE));
	SetWidthLen(fWidthLen);
	SetHeightLen(fHeightLen);

	if (m_nLifeMax - m_nLife == TIME_ADDSIZE)
	{
		m_state = STATE_DMG;
		return;
	}

	// 状態遷移カウンター減算
	m_nCntState--;
	if (m_nCntState <= 0)
	{
		m_nCntState = 0;
		m_state = STATE_NONE;
	}
}

//==========================================================================
// ダメージ状態処理
//==========================================================================
void CExplosion::StateDamage()
{
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	fWidthLen += 0.5f;
	fHeightLen += 0.5f;
	SetWidthLen(fWidthLen);
	SetHeightLen(fHeightLen);

	// 当たり判定処理
	(this->*(m_CollisionFuncList[m_type]))();
}

//==========================================================================
// フェードアウト状態処理
//==========================================================================
void CExplosion::StateFadeout()
{
	// 色取得
	D3DXCOLOR col = GetColor();

	// 不透明度更新
	col.a = static_cast<float>(m_nLife + TIME_FADEOUT) / static_cast<float>(TIME_FADEOUT);

	// 色設定
	SetColor(col);

	if (m_nLife + TIME_FADEOUT <= 0)
	{
		// 終了処理
		Uninit();
		return;
	}

}

//==========================================================================
// プレイヤーとの判定
//==========================================================================
void CExplosion::CollisionPlayer()
{

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		// プレイヤーの情報取得
		MyLib::Vector3 PlayerPosition = pPlayer->GetCenterPosition();
		MyLib::Vector3 PlayerRotation = pPlayer->GetRotation();
		float fPlayerRadius = pPlayer->GetRadius();

		// 情報取得
		MyLib::Vector3 pos = GetPosition();
		float fRadius = GetWidthLen();

		if (UtilFunc::Collision::SphereRange(pos, PlayerPosition, fRadius, fPlayerRadius).ishit)
		{// 当たっていたら

			// ヒット処理
			pPlayer->Hit(1);

			// 終了処理
			//Uninit();
			break;
		}
	}

}

//==========================================================================
// 敵との判定
//==========================================================================
void CExplosion::CollisionEnemy()
{
	// 情報取得
	MyLib::Vector3 pos = GetPosition();
	float fRadius = GetWidthLen();
	bool bHit = false;

	// 敵のリスト取得
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	// リストループ
	while (enemyList.ListLoop(&pEnemy))
	{
		// 敵の情報取得
		MyLib::Vector3 EnemyPosition = pEnemy->GetCenterPosition();
		float fEnemyRadius = pEnemy->GetRadius();

		if (UtilFunc::Collision::SphereRange(pos, EnemyPosition, fRadius, fEnemyRadius).ishit)
		{// 当たっていたら
			bHit = true;
			break;
		}
	}

	if (bHit == true)
	{// 当たってたら

		// 終了処理
		Uninit();
		return;
	}

}

//==========================================================================
// 描画処理
//==========================================================================
void CExplosion::Draw()
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// ビルボードの描画
	CMeshSphere::Draw();

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CExplosion::SetVtx()
{
	// 頂点設定
	CMeshSphere::SetVtx();
}

//==========================================================================
// 状態設定
//==========================================================================
void CExplosion::SetState(STATE state, int nCntState)
{
	m_state = state;			// 状態
	m_nCntState = nCntState;	// 状態遷移カウンター
}

//==========================================================================
// 状態取得
//==========================================================================
CExplosion::STATE CExplosion::GetState()
{
	return m_state;
}

//==========================================================================
// 種類取得
//==========================================================================
CExplosion::TYPE CExplosion::GetType()
{
	return m_type;
}
