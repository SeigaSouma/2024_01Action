//=============================================================================
// 
//  障害物弾処理 [bullet_obstacle.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "bullet_obstacle.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "3D_effect.h"
#include "sound.h"
#include "particle.h"
#include "calculation.h"
#include "player.h"


//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* BULLETMODEL = "data\\MODEL\\attack\\obstacleballet_01.x";	// モデルファイル
	const int DAMAGE = 8;	// ダメージ
}

//==========================================================================
// コンストラクタ
//==========================================================================
CBulletObstacle::CBulletObstacle(int nPriority) : CObjectX(nPriority)
{
	// 値のクリア
	m_fRadius = 0.0f;		// 半径
	m_nCntMove = 0;	// 移動カウンター
}

//==========================================================================
// デストラクタ
//==========================================================================
CBulletObstacle::~CBulletObstacle()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CBulletObstacle* CBulletObstacle::Create(const MyLib::Vector3& pos, const MyLib::Vector3& rot, const D3DXVECTOR2& paraboramove, const float size)
{

	// メモリの確保
	CBulletObstacle* pBullet = DEBUG_NEW CBulletObstacle;

	if (pBullet != NULL)
	{// メモリの確保が出来ていたら

		// 位置割り当て
		pBullet->SetPosition(pos);
		pBullet->SetOriginPosition(pos);

		// 位置割り当て
		pBullet->SetRotation(rot);

		// 移動量設定

		MyLib::Vector3 move;
		move.y = paraboramove.y;
		move.x = sinf(D3DX_PI + rot.y) * paraboramove.x;
		move.z = cosf(D3DX_PI + rot.y) * paraboramove.x;

		pBullet->SetMove(move);

		// サイズ設定
		pBullet->m_fRadius = size;

		// 初期化処理
		pBullet->Init();
	}

	return pBullet;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBulletObstacle::Init(void)
{
	// 種類の設定
	CObject::SetType(TYPE_BULLET);

	// 初期化処理
	HRESULT hr = CObjectX::Init(BULLETMODEL);
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBulletObstacle::Uninit(void)
{
	// 終了処理
	CObjectX::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CBulletObstacle::Update(void)
{
	// 位置更新
	UpdatePos();

	// プレイヤーとの判定
	CollisionPlayer();

	// 位置制限
	LimitPos();
}

//==========================================================================
// 移動処理
//==========================================================================
void CBulletObstacle::UpdatePos(void)
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 posOrigin = GetOriginPosition();

	// 移動量取得
	MyLib::Vector3 move = GetMove();

	// 位置更新
	m_nCntMove++;
	pos = (MyLib::Vector3(0.0f, -0.4f, 0.0f) * (float)(m_nCntMove * m_nCntMove) + move * (float)m_nCntMove) + posOrigin;

	// 位置設定
	SetPosition(pos);

#if _DEBUG
	CEffect3D::Create(
		pos,
		MyLib::Vector3(0.0f, 0.0f, 0.0f),
		D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),
		m_fRadius, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
#endif
}

//==========================================================================
// 位置制限
//==========================================================================
void CBulletObstacle::LimitPos(void)
{
	MyLib::Vector3 pos = GetPosition();

	float fLength = sqrtf(pos.x * pos.x + pos.z * pos.z);

	if (fLength > mylib_const::RADIUS_STAGE)
	{
		Uninit();
		return;
	}

	if (m_nCntMove > 4 &&
		pos.y <= 0.0f)
	{
		Uninit();
		return;
	}
}

//==========================================================================
// プレイヤーとの判定
//==========================================================================
void CBulletObstacle::CollisionPlayer(void)
{
	// プレイヤー情報取得
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
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

	if (UtilFunc::Collision::SphereRange(pos, PlayerPosition, m_fRadius, fPlayerRadius).ishit)
	{// 当たっていたら

		// ヒット処理
		if (pPlayer->Hit(DAMAGE))
		{
			// 終了処理
			Uninit();
		}
	}

}

//==========================================================================
// 描画処理
//==========================================================================
void CBulletObstacle::Draw(void)
{
	// 描画
	CObjectX::Draw();
}

