//=============================================================================
// 
//  弾処理 [bullet.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "bullet.h"
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
#include "effect_thunderring.h"

//==========================================================================
//  定数定義
//==========================================================================
namespace
{
	const int MAX_LIFE = 60 * 2;			// 最大寿命
	const float DEFAULT_HEIGHT = 400.0f;	// デフォの放物線高さ
}

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CBullet::m_apTextureFile[TYPE_MAX] =	// テクスチャのファイル
{
	"",
	"",
};
int CBullet::m_nNumAll = 0;		// 弾の総数
CListManager<CBullet> CBullet::m_List = {};	// リスト

//==========================================================================
// 関数ポインタ
//==========================================================================
CBullet::STATE_FUNC CBullet::m_FuncList[] =
{
	&CBullet::StateNone,
	&CBullet::StateDamage,
};

CBullet::COLLISION_FUNC CBullet::m_CollisionFuncList[] =	// 当たり判定のリスト
{
	&CBullet::CollisionEnemy,	// 敵との判定
	&CBullet::CollisionPlayer,	// プレイヤーとの判定
};

//==========================================================================
// コンストラクタ
//==========================================================================
CBullet::CBullet(int nPriority) : CMeshSphere(nPriority)
{
	// 値のクリア
	m_type = TYPE_PLAYER;
	m_state = STATE_NONE;			// 状態
	m_MoveType = MOVETYPE_NORMAL;	// 移動の種類
	m_OriginPosition = mylib_const::DEFAULT_VECTOR3;	// 元の位置
	m_TargetPosition = mylib_const::DEFAULT_VECTOR3;	// 目標の位置
	m_nCntState = 0;				// 状態遷移カウンター
	m_nLife = 0;
	m_nCntEmission = 0;				// 発生物のカウンター
	m_fMaxParabolaHeight = 0.0f;	// 放物線の最大高さ
	m_bAutoDeath = false;	// 自動削除のフラグ
	m_bFinish = false;		// アニメーションが終わった判定

	// テクスチャデータの配列分繰り返す
	m_nTexIdx = 0;		// テクスチャのインデックス番号

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CBullet::~CBullet()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CBullet *CBullet::Create(TYPE type, MOVETYPE movetype, const MyLib::Vector3 pos, const MyLib::Vector3 rot, const MyLib::Vector3 move, const float fSize)
{
	// 生成用のオブジェクト
	CBullet *pBullet = NULL;

	if (pBullet == NULL)
	{// NULLだったら

		// メモリの確保
		pBullet = DEBUG_NEW CBullet;

		if (pBullet != NULL)
		{// メモリの確保が出来ていたら

			// 種類設定
			pBullet->m_type = type;

			// 移動種類
			pBullet->m_MoveType = movetype;

			// 位置割り当て
			pBullet->SetPosition(pos);
			pBullet->m_OriginPosition = pos;

			// 位置割り当て
			pBullet->SetRotation(rot);

			// 移動量設定
			pBullet->SetMove(move);

			// サイズ設定
			pBullet->SetWidthLen(fSize);
			pBullet->SetHeightLen(fSize);
			if (fSize >= 40.0f)
			{
				pBullet->SetWidthBlock(5);
				pBullet->SetHeightBlock(5);
			}
			else
			{
				pBullet->SetWidthBlock(16);
				pBullet->SetHeightBlock(16);
			}

			// 初期化処理
			pBullet->Init();
		}

		return pBullet;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBullet::Init(void)
{
	HRESULT hr;

	// 各種変数の初期化
	m_nLifeMax = MAX_LIFE;
	m_nLife = m_nLifeMax;	// 寿命
	SetColor(D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f));
	m_fMaxParabolaHeight = DEFAULT_HEIGHT;	// 放物線の最大高さ
	m_bAutoDeath = true;	// 自動削除にする

	// テクスチャの割り当て
	m_nTexIdx = CTexture::GetInstance()->Regist(m_apTextureFile[m_type]);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 種類の設定
	CObject::SetType(TYPE_BULLET);

	// 初期化処理
	hr = CMeshSphere::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	// リストに追加
	m_List.Regist(this);

	// テクスチャの割り当て
	int nTex = CTexture::GetInstance()->Regist("data\\TEXTURE\\effect\\effect000.jpg");

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBullet::Uninit(void)
{
	
	// 削除
	m_List.Delete(this);

	// 終了処理
	CMeshSphere::Uninit();

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CBullet::Update(void)
{
	if (GetHeightLen() < 40.0f)
	{// 重力
		MyLib::Vector3 move = GetMove();

		move.y -= 0.16f;

		SetMove(move);
	}

	// 位置更新
	UpdatePos();

	// 状態別処理
	(this->*(m_FuncList[m_state]))();

	// 当たり判定処理
	(this->*(m_CollisionFuncList[m_type]))();

	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	if (m_type == TYPE_ENEMY)
	{
		m_nCntEmission = (m_nCntEmission + 1) % 100;	// 発生物のカウンター

	}
	else if (m_type == TYPE_PLAYER)
	{
	}

	// 寿命減算
	m_nLife--;

	if (m_nLife <= 0)
	{// 寿命が尽きたら

		m_bFinish = true;

		if (m_bAutoDeath)
		{
			Uninit();
		}
		return;
	}

	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 移動処理
//==========================================================================
void CBullet::UpdatePos(void)
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// 向き取得
	MyLib::Vector3 rot = GetRotation();

	// 位置更新
	switch (m_MoveType)
	{
	case CBullet::MOVETYPE_NORMAL:
		pos += move;
		break;

	case CBullet::MOVETYPE_PARABOLA:	// 放物線移動
		pos = UtilFunc::Calculation::GetParabola3D(m_OriginPosition, m_TargetPosition, m_fMaxParabolaHeight, 1.0f - ((float)m_nLife / (float)m_nLifeMax));
		break;
	}

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
void CBullet::StateNone(void)
{
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
void CBullet::StateDamage(void)
{
	// 状態遷移カウンター減算
	m_nCntState--;
	if (m_nCntState <= 0)
	{
		m_nCntState = 0;
		m_state = STATE_NONE;

	}
}

//==========================================================================
// プレイヤーとの判定
//==========================================================================
void CBullet::CollisionPlayer(void)
{
	// プレイヤー情報取得
	for (int nCntPlayer = 0; nCntPlayer < mylib_const::MAX_PLAYER; nCntPlayer++)
	{
		CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer(nCntPlayer);
		if (pPlayer == NULL)
		{// NULLだったら
			return;
		}

		// プレイヤーの情報取得
		MyLib::Vector3 PlayerPosition = pPlayer->GetCenterPosition();
		MyLib::Vector3 PlayerRotation = pPlayer->GetRotation();
		float fPlayerRadius = pPlayer->GetRadius();

		// 情報取得
		MyLib::Vector3 pos = GetPosition();
		float fRadius = GetWidthLen();

		if (UtilFunc::Collision::SphereRange(pos, PlayerPosition, fRadius, fPlayerRadius))
		{// 当たっていたら

			// ヒット処理
			pPlayer->Hit(1);

			m_bFinish = true;

			if (m_bAutoDeath)
			{
				Uninit();
			}
			break;
		}
	}
}

//==========================================================================
// 敵との判定
//==========================================================================
void CBullet::CollisionEnemy(void)
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

		if (UtilFunc::Collision::SphereRange(pos, EnemyPosition, fRadius, fEnemyRadius))
		{// 当たっていたら
			bHit = true;

			pEnemy->Hit(10);

			break;
		}
	}

	if (bHit == true)
	{// 当たってたら

		m_bFinish = true;

		if (GetHeightLen() < 40.0f)
		{// 重力			
			Uninit();
		}

		return;
	}
}

//==========================================================================
// プレイヤー弾の更新
//==========================================================================
void CBullet::UpdateTypePlayer(void)
{

}

//==========================================================================
// 描画処理
//==========================================================================
void CBullet::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// ビルボードの描画
	CMeshSphere::Draw();

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);


	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CBullet::SetVtx(void)
{
	// 頂点設定
	CMeshSphere::SetVtx();
}

//==========================================================================
// 状態設定
//==========================================================================
void CBullet::SetState(STATE state, int nCntState)
{
	m_state = state;			// 状態
	m_nCntState = nCntState;	// 状態遷移カウンター
}

//==========================================================================
// 状態取得
//==========================================================================
CBullet::STATE CBullet::GetState(void)
{
	return m_state;
}

//==========================================================================
// 種類設定
//==========================================================================
void CBullet::SetType(TYPE type)
{
	m_type = type;

	// テクスチャの割り当て
	m_nTexIdx = CTexture::GetInstance()->Regist(m_apTextureFile[m_type]);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);
}

//==========================================================================
// 種類取得
//==========================================================================
CBullet::TYPE CBullet::GetType(void)
{
	return m_type;
}

//==========================================================================
// 目標の位置
//==========================================================================
void CBullet::SetTargetPosition(MyLib::Vector3 pos)
{
	m_TargetPosition = pos;
}

//==========================================================================
// 放物線の最大高さ
//==========================================================================
void CBullet::SetParabolaHeight(float fHeight)
{
	m_fMaxParabolaHeight = fHeight;
}

//==========================================================================
// 総数取得
//==========================================================================
int CBullet::GetNumAll(void)
{
	return m_nNumAll;
}
