//=============================================================================
// 
//  ビームヘッダー [beam.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BEAM_H_
#define _BEAM_H_	// 二重インクルード防止

#include "object.h"
#include "collisionobject.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CEffect3D;

namespace my_beam
{
	const int DEFAULTDENSITY_BEAM = 12;	// ビームの密度
}

//==========================================================================
// クラス定義
//==========================================================================
// ビームクラス定義
class CBeam : public CObject
{
public:

	// ビームの種類列挙
	enum eBeamType
	{
		TYPE_NORMAL = 0,	// 通常
		TYPE_RESIDUAL,		// 残留
		TYPE_MAX
	};

	CBeam(int nPriority = mylib_const::PRIORITY_ZSORT);
	~CBeam();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	static CBeam *Create(const MyLib::Vector3 pos, const MyLib::Vector3 move, const D3DXCOLOR col, const float fRadius, const float fLength, const int nLife, const int nDisity, const int nDamage, CCollisionObject::eMyTag TagType, eBeamType BeamType = TYPE_NORMAL);
	void SetDisableAddAlpha();	// 加算合成取り消し

private:

	// メンバ関数
	void UpdateEffect();
	void UpdateBillboard();

	// メンバ変数
	float m_fRadius;		// 半径
	float m_fLength;		// 長さ
	float m_fDestLength;	// 目標の長さ
	float m_moveRatio;		// 移動の割合
	int m_nDisity;			// 密度
	int m_nDamage;			// ダメージ
	D3DXCOLOR m_color;		// 色
	std::vector<CEffect3D*> m_pEffect;	// エフェクトのオブジェクト
	std::vector<CObjectBillboard*> m_pObjBillboard;	// ビルボードのオブジェクト
	CCollisionObject::eMyTag m_Tag;	// タグ
	eBeamType m_BeamType;	// ビームの種類
	int m_nLife;	// 寿命
	int m_nLifeOrigin;	// 寿命
	int m_nCntEmission;		// 発生物のカウンター
	static int m_nTexIdx;	// テクスチャのインデックス番号
};


#endif