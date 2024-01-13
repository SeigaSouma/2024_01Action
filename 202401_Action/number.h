//=============================================================================
// 
//  数字ヘッダー [number.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _NUMBER_H_
#define _NUMBER_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"
#include "object3D.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObject2D;
class CObjectBillboard;

//==========================================================================
// クラス定義
//==========================================================================
// 数字クラス定義
class CNumber
{
public:

	// 列挙型定義
	enum EObjectType
	{
		OBJECTTYPE_2D = 0,		// 2D
		OBJECTTYPE_3D,			// 3D
		OBJECTTYPE_BILLBOARD,	// ビルボード
		OBJECTTYPE_MAX
	};

	CNumber(int nPriority = 6);
	~CNumber();

	static CNumber *Create(EObjectType objtype, int nPriority);

	// メンバ関数
	virtual HRESULT Init(int nPriority) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	virtual void Release(void) = 0;	// 開放処理


	virtual void SetPosition(const MyLib::Vector3 pos);	// 位置設定
	virtual MyLib::Vector3 GetPosition(void) const;		// 位置取得
	virtual void SetMove(const MyLib::Vector3 move);		// 移動量設定
	virtual MyLib::Vector3 GetMove(void) const;			// 移動量取得
	virtual void SetRotation(const MyLib::Vector3 rot);	// 向き設定
	virtual MyLib::Vector3 GetRotation(void) const;		// 向き取得

	virtual void SetColor(const D3DXCOLOR col);			// 色設定
	virtual D3DXCOLOR GetColor(void) const;				// 色取得
	virtual void SetSize(const D3DXVECTOR2 size);		// サイズの設定
	virtual D3DXVECTOR2 GetSize(void) const;			// サイズの取得
	virtual void SetSizeOrigin(const D3DXVECTOR2 size);	// 元のサイズの設定
	virtual D3DXVECTOR2 GetSizeOrigin(void) const;		// 元のサイズの取得
	virtual void SetSize3D(const MyLib::Vector3 size);		// サイズの設定
	virtual MyLib::Vector3 GetSize3D(void) const;			// サイズの取得
	virtual void SetTex(D3DXVECTOR2 *tex);				// テクスチャ座標の設定
	virtual D3DXVECTOR2 *GetTex(void);					// テクスチャ座標の取得

	virtual void SetVtx(void) = 0;
	virtual void BindTexture(int nIdx) = 0;
	virtual void SetType(const CObject::TYPE type) = 0;

	virtual CObject2D *GetObject2D(void);
	virtual CObject3D *GetObject3D(void);
	virtual CObjectBillboard *GetObjectBillboard(void);

private:
	EObjectType m_objType;				// オブジェクトの種類
};

#endif