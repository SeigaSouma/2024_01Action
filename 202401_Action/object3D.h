//=============================================================================
// 
//  オブジェクト3Dヘッダー [object3D.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// クラス定義
//==========================================================================
// オブジェクト3Dクラス定義
class CObject3D : public CObject
{
public:

	CObject3D(int nPriority = 5);
	virtual ~CObject3D();

	// オーバーライドされた関数
	virtual HRESULT Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();
	virtual void SetVtx();
	virtual void BindTexture(int nIdx);

	void SetWorldMtx(const D3DXMATRIX mtx);		// マトリックス設定
	D3DXMATRIX GetWorldMtx() const;			// マトリックス取得
	void SetOriginPosition(const MyLib::Vector3 pos);	//	元の位置設定
	MyLib::Vector3 GetOriginPosition() const;		//	元の位置取得
	void SetOriginRotation(const MyLib::Vector3 rot);	// 元の向き設定
	MyLib::Vector3 GetOriginRotation() const;		// 元の向き取得
	void SetColor(const D3DXCOLOR col);			// 色設定
	D3DXCOLOR GetColor() const;				// 色取得
	void SetSize(const MyLib::Vector3 size);		// サイズの設定
	MyLib::Vector3 GetSize() const;			// サイズの取得
	virtual void SetTex(D3DXVECTOR2 *tex);				// テクスチャ座標の設定
	virtual D3DXVECTOR2 *GetTex();					// テクスチャ座標の取得

	static CObject3D *Create(int nPriority);
	static CObject3D *Create(MyLib::Vector3 pos, MyLib::Vector3 rot);
	CObject3D *GetObject3D();
	virtual CObject3DMesh *GetObject3DMesh();

protected:

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファ

private:
	D3DXMATRIX	m_mtxWorld;		// ワールドマトリックス
	MyLib::Vector3 m_posOrigin;	// 元の位置
	MyLib::Vector3 m_rotOrigin;	// 元の向き
	D3DXVECTOR2 m_fTex[32];		// テクスチャ座標
	D3DXCOLOR m_col;			// 色
	MyLib::Vector3 m_fSize;		// サイズ
	int m_nTexIdx;					// テクスチャのインデックス番号
};



#endif