//=============================================================================
// 
//  オブジェクト2Dヘッダー [object2D_Multi.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _OBJECT2D_MULTI_H_
#define _OBJECT2D_MULTI_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

class CEnemy;

//==========================================================================
// クラス定義
//==========================================================================
// オブジェクト2Dクラス定義
class CObject2DMulti : public CObject
{
public:

	CObject2DMulti(int nPriority = mylib_const::PRIORITY_DEF2D);
	~CObject2DMulti();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Draw(int nNumVertex);
	void Draw(LPDIRECT3DTEXTURE9 mutitex);
	virtual void SetVtx(void);
	void BindTexture(int nIdx);
	int GetIdxTexture(void) { return m_nTexIdx; }

	virtual void SetColor(const D3DXCOLOR col);			// 色設定
	virtual D3DXCOLOR GetColor(void) const;				// 色取得
	virtual void SetSize(const D3DXVECTOR2 size);		// サイズの設定
	virtual D3DXVECTOR2 GetSize(void) const;			// サイズの取得
	virtual void SetSizeOrigin(const D3DXVECTOR2 size);	// 元のサイズの設定
	virtual D3DXVECTOR2 GetSizeOrigin(void) const;		// 元のサイズの取得
	virtual void SetTex(D3DXVECTOR2 *tex);				// テクスチャ座標の設定
	virtual D3DXVECTOR2 *GetTex(void);					// テクスチャ座標の取得
	virtual MyLib::Vector3 *GetVtxPos(void);				// 頂点座標取得
	virtual void SetVtxPos(MyLib::Vector3 *pos);			// 頂点座標取得
	

	static CObject2DMulti *Create();
	static CObject2DMulti *Create(int nPriority);
	CObject2DMulti * GetObject2DMulti(void);
	LPDIRECT3DVERTEXBUFFER9 GetVtxBuff(void) const;
protected:

private:

	void SetLength(const float fLength);		// 対角線の長さ設定
	float GetLength(void) const;				// 対角線の長さ取得
	void SetAngle(const float fAngle);			// 対角線の向き設定
	float GetAngle(void) const;					// 対角線の向き取得

	D3DXCOLOR m_col;			// 色
	D3DXVECTOR2 m_size;			// サイズ
	D3DXVECTOR2 m_sizeOrigin;	// 元のサイズ
	D3DXVECTOR2 m_fTex[32];		// テクスチャ座標
	float m_fLength;			// 対角線の長さ
	float m_fAngle;				// 対角線の向き
	MyLib::Vector3 m_VtxPos[32];		// 頂点座標
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファ
	int m_nTexIdx;					// テクスチャのインデックス番号
};

#endif