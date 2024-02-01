//=============================================================================
// 
//  ビーム処理 [beam.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "beam.h"
#include "game.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "3D_effect.h"
#include "camera.h"
#include "particle.h"
#include "ballast.h"
#include "texture.h"
#include "impactwave.h"

namespace
{
	const char* TEXTURE = "data\\TEXTURE\\effect\\effect000.jpg";
	const float RATIO_SUBSIZE = 0.3f;	// サイズ縮小の割合
	const float TIME_LENGTH = 10;	// 長さ補正の時間
}

int CBeam::m_nTexIdx = 0;		// テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CBeam::CBeam(int nPriority) : CObject(nPriority)
{
	m_fRadius = 0.0f;		// 半径
	m_fLength = 0.0f;		// 長さ
	m_fDestLength = 0.0f;	// 目標の長さ
	m_moveRatio = 0.0f;		// 移動の割合
	m_nDisity = 0;			// 密度
	m_nDamage = 0;			// ダメージ
	m_color = mylib_const::DEFAULT_COLOR;	// 色
	m_pEffect.clear();	// エフェクトのオブジェクト
	m_pObjBillboard.clear();	// ビルボードのオブジェクト
	m_Tag = CCollisionObject::TAG_NONE;	// タグ
	m_nLife = 0;	// 寿命
	m_nLifeOrigin = 0;	// 寿命
	m_nCntEmission = 0;	// 発生物のカウンター
}

//==========================================================================
// デストラクタ
//==========================================================================
CBeam::~CBeam()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CBeam *CBeam::Create(
	const MyLib::Vector3 pos, const MyLib::Vector3 move, const D3DXCOLOR col,
	const float fRadius, const float fLength, const int nLife,
	const int nDisity, const int nDamage, CCollisionObject::eMyTag TagType, eBeamType BeamType)
{
	// 生成用のオブジェクト
	CBeam *pBallast = NULL;

	if (pBallast == NULL)
	{// NULLだったら

		// メモリの確保
		pBallast = DEBUG_NEW CBeam;

		if (pBallast != NULL)
		{// メモリの確保が出来ていたら

			pBallast->m_fRadius = fRadius;	// 半径
			pBallast->m_fLength = fLength;	// 長さ
			pBallast->m_fDestLength = fLength;	// 目標の長さ
			pBallast->m_nLife = nLife;		// 寿命
			pBallast->SetPosition(pos);		// 位置
			pBallast->SetMove(move);		// 移動量
			pBallast->m_color = col;		// 色
			pBallast->m_nDisity = nDisity;	// 密度
			pBallast->m_nDamage = nDamage;	// ダメージ
			pBallast->m_Tag = TagType;		// タグ
			pBallast->m_BeamType = BeamType;	// ビームの種類

			// 初期化処理
			HRESULT hr = pBallast->Init();
			if (FAILED(hr))
			{
				return NULL;
			}
		}

		return pBallast;
	}

	return NULL;
}

//==========================================================================
// 加算合成取り消し
//==========================================================================
void CBeam::SetDisableAddAlpha(void)
{
	for (int i = 0; i < static_cast<int>(m_pEffect.size()); i++)
	{
		if (m_pEffect[i] == nullptr)
		{
			continue;
		}

		m_pEffect[i]->SetDisableAddAlpha();
	}
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBeam::Init(void)
{
	// 種類の設定
	SetType(TYPE_OBJECT3D);

	// 位置取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 move = GetMove();
	MyLib::Vector3 vecmove = move - mylib_const::DEFAULT_VECTOR3;

	m_nLifeOrigin = m_nLife;

	// ベクトルを正規化
	D3DXVec3Normalize(&vecmove, &vecmove);

	if (m_BeamType == TYPE_RESIDUAL)
	{
		m_fLength = 0.0f;
	}

	float fDistance = m_fLength / (float)m_nDisity;
	float fLen = 0.0f;
	for (int nCntBallast = 0; nCntBallast < m_nDisity; nCntBallast++)
	{
		switch (m_BeamType)
		{
		case CBeam::TYPE_NORMAL:
		{
			// 生成処理
			CEffect3D *pEffect = CEffect3D::Create(
				pos + vecmove * fLen,
				move,
				m_color,
				m_fRadius,
				m_nLife,
				CEffect3D::MOVEEFFECT_SUB,
				CEffect3D::TYPE_NORMAL,
				0.0f);

			// エフェクト追加
			m_pEffect.push_back(pEffect);
		}
			break;

		case CBeam::TYPE_RESIDUAL:
		{
			// 生成処理
			CObjectBillboard *pEffect = CObjectBillboard::Create(pos + vecmove * fLen, mylib_const::DEFAULT_VECTOR3);
			pEffect->SetMove(move);
			pEffect->SetColor(m_color);
			pEffect->SetSize(D3DXVECTOR2(m_fRadius, m_fRadius));
			pEffect->SetSizeOrigin(D3DXVECTOR2(m_fRadius, m_fRadius));
			pEffect->SetType(CObject::TYPE_NONE);

			if (m_nTexIdx == 0)
			{
				m_nTexIdx = CTexture::GetInstance()->Regist(TEXTURE);
			}
			pEffect->BindTexture(m_nTexIdx);

			// ビルビード追加
			m_pObjBillboard.push_back(pEffect);
		}
			break;
		}

		// 当たり判定オブジェクト生成
		CCollisionObject::Create(pos + vecmove * fLen, move, m_fRadius, m_nLife, m_nDamage, m_Tag);

		// 距離加算
		fLen += fDistance;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBeam::Uninit(void)
{
	// 要素全削除
	m_pEffect.clear();

	for (int i = 0; i < static_cast<int>(m_pObjBillboard.size()); i++)
	{
		if (m_pObjBillboard[i] == nullptr)
		{
			continue;
		}
		m_pObjBillboard[i]->Uninit();
		m_pObjBillboard[i] = nullptr;
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CBeam::Update(void)
{
	// 寿命減算
	m_nLife--;

	if (m_nLife <= 0)
	{
		Uninit();
		return;
	}

	switch (m_BeamType)
	{
	case CBeam::TYPE_NORMAL:
		UpdateEffect();
		break;

	case CBeam::TYPE_RESIDUAL:
		UpdateBillboard();
		break;
	}
	
}

//==========================================================================
// エフェクトの更新
//==========================================================================
void CBeam::UpdateEffect(void)
{
	for (int i = 0; i < static_cast<int>(m_pEffect.size()); i++)
	{
		if (m_pEffect[i] == nullptr)
		{
			continue;
		}

		// 位置取得
		MyLib::Vector3 pos = m_pEffect[i]->GetPosition();

		if (CGame::GetInstance()->GetElevation()->IsHit(pos) == true)
		{
			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(6, 4.0f, 0.0f);

			// 瓦礫
			CBallast::Create(pos, MyLib::Vector3(2.0f, 6.0f, 2.0f), 1, 1.0f, CBallast::TYPE_STONE);

			// ビームヒットパーティクル
			my_particle::Create(pos, my_particle::TYPE_BEAMHIT_FIELD);

			m_pEffect[i]->Uninit();
			m_pEffect[i] = nullptr;

			// 着地したものを削除
			m_pEffect.erase(m_pEffect.begin() + i);
		}
	}
}

//==========================================================================
// ビルビードの更新
//==========================================================================
void CBeam::UpdateBillboard(void)
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 move = GetMove();
	pos += move;
	SetPosition(pos);

	float fAlpha = (float)m_nLife / (float)m_nLifeOrigin;
	float fSubRatio = (float)(m_nLife + (m_nLifeOrigin * (1.0f - RATIO_SUBSIZE))) / (float)m_nLifeOrigin;
	UtilFunc::Transformation::ValueNormalize(fSubRatio, 1.0f, 0.0f);

	// 要素分繰り返し
	int nCntBillboard = 0;

	// 長さ補正
	//UtilFunc::Correction::InertiaCorrection(m_fLength, m_fDestLength, 0.25f);

	if (m_moveRatio < 1.0f)
	{
		m_moveRatio = (float)(m_nLifeOrigin - m_nLife) / TIME_LENGTH;
		m_fLength = UtilFunc::Correction::EasingEaseOut(0.0f, m_fDestLength, m_moveRatio);
		UtilFunc::Transformation::ValueNormalize(m_moveRatio, 1.0f, 0.0f);

		// 発生物カウンター加算
		m_nCntEmission = (m_nCntEmission + 1) % 1;

		if (m_nCntEmission == 0)
		{
			// 先端の位置
			MyLib::Vector3 spawnpos = pos + (move * m_fLength);
			float moverot = atan2f(move.x, move.z);

			// 衝撃波生成
			CImpactWave::Create
			(
				spawnpos,	// 位置
				MyLib::Vector3(D3DX_PI * 0.5f, moverot, 0.0f),				// 向き
				mylib_const::PLAYERBEAM_COLOR,			// 色
				50.0f,						// 幅
				20.0f,						// 高さ
				60.0f,						// 中心からの距離
				15,							// 寿命
				40.0f,						// 幅の移動量
				CImpactWave::TYPE_PURPLE4,	// テクスチャタイプ
				true						// 加算合成するか
			);
		}
	}

	float fDistance = m_fLength / (float)m_nDisity;
	float fLen = 0.0f;

	for (int i = static_cast<int>(m_pObjBillboard.size()) - 1; i >= 0; --i)
	{
		const auto& billboard = m_pObjBillboard[i];

		if (billboard == nullptr)
		{
			continue;
		}

		// 色
		D3DXCOLOR col = billboard->GetColor();
		col.a = fAlpha;
		billboard->SetColor(col);

		// 位置取得
		MyLib::Vector3 posbillboard = billboard->GetPosition();

		// サイズ取得
		D3DXVECTOR2 sizeorigin = billboard->GetSizeOrigin();

		// 位置更新
		posbillboard = pos + move * fLen;
		billboard->SetPosition(posbillboard);

		// サイズ設定
		m_fRadius = sizeorigin.x * fSubRatio;
		billboard->SetSize(D3DXVECTOR2(m_fRadius, m_fRadius));

		// 距離加算
		fLen += fDistance;

		if (CGame::GetInstance()->GetElevation()->IsHit(pos) == true)
		{
			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(6, 4.0f, 0.0f);

			// 瓦礫
			CBallast::Create(pos, MyLib::Vector3(2.0f, 6.0f, 2.0f), 1, 1.0f, CBallast::TYPE_STONE);

			// ビームヒットパーティクル
			my_particle::Create(pos, my_particle::TYPE_BEAMHIT_FIELD);

			billboard->Uninit();
			//billboard = nullptr;

			// 着地したものを削除
			m_pObjBillboard.erase(m_pObjBillboard.begin() + i);
			continue;
		}

		// 更新
		billboard->Update();
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CBeam::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// リストコピー
	std::vector<CObjectBillboard*> pObjectSort = m_pObjBillboard;

	// Zソート
	std::sort(pObjectSort.begin(), pObjectSort.end(), ZSort);

	for (int i = 0; i < (int)pObjectSort.size(); i++)
	{
		if (pObjectSort[i] == nullptr)
		{
			continue;
		}
		pObjectSort[i]->Draw();
	}

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);


}
