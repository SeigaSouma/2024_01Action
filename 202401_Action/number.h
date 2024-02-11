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
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Release() = 0;	// 開放処理


	virtual void SetPosition(const MyLib::Vector3 pos);	// 位置設定
	virtual MyLib::Vector3 GetPosition() const;		// 位置取得
	virtual void SetMove(const MyLib::Vector3 move);		// 移動量設定
	virtual MyLib::Vector3 GetMove() const;			// 移動量取得
	virtual void SetRotation(const MyLib::Vector3 rot);	// 向き設定
	virtual MyLib::Vector3 GetRotation() const;		// 向き取得

	virtual void SetColor(const D3DXCOLOR col);			// 色設定
	virtual D3DXCOLOR GetColor() const;				// 色取得
	virtual void SetSize(const D3DXVECTOR2 size);		// サイズの設定
	virtual D3DXVECTOR2 GetSize() const;			// サイズの取得
	virtual void SetSizeOrigin(const D3DXVECTOR2 size);	// 元のサイズの設定
	virtual D3DXVECTOR2 GetSizeOrigin() const;		// 元のサイズの取得
	virtual void SetSize3D(const MyLib::Vector3 size);		// サイズの設定
	virtual MyLib::Vector3 GetSize3D() const;			// サイズの取得
	virtual void SetTex(D3DXVECTOR2 *tex);				// テクスチャ座標の設定
	virtual D3DXVECTOR2 *GetTex();					// テクスチャ座標の取得

	virtual void SetVtx() = 0;
	virtual void BindTexture(int nIdx) = 0;
	virtual void SetType(const CObject::TYPE type) = 0;

	virtual CObject2D *GetObject2D();
	virtual CObject3D *GetObject3D();
	virtual CObjectBillboard *GetObjectBillboard();

private:
	EObjectType m_objType;				// オブジェクトの種類
};

#endif