//=============================================================================
// 
//  オブジェクトXヘッダー [objectX.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _OBJECTX_H_
#define _OBJECTX_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"
#include "Xload.h"

// 前方宣言
class CShadow;

//==========================================================================
// クラス定義
//==========================================================================
// オブジェクトXクラス定義
class CObjectX : public CObject
{
public:

	// マクロ定義
#define MAX_MAT				(512)		// マテリアルサイズ
#define MAX_TX				(96)		// テクスチャサイズ

	CObjectX(int nPriority = mylib_const::PRIORITY_DEFAULT);
	~CObjectX();

	// オーバーライドされた関数
	HRESULT Init(void);
	HRESULT Init(const char *pFileName);
	HRESULT Init(int nIdxXFile);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Draw(D3DXCOLOR col);
	void Draw(float fAlpha);
	void BindTexture(int *nIdx);
	void BindXData(int nIdxXFile);

	void SetWorldMtx(const D3DXMATRIX mtx);		// マトリックス設定
	D3DXMATRIX GetWorldMtx(void) const;			// マトリックス取得
	void SetScale(const MyLib::Vector3 scale);		// スケール設定
	MyLib::Vector3 GetScale(void) const;			// スケール取得
	void SetColor(const D3DXCOLOR col);			// 色設定
	D3DXCOLOR GetColor(void) const;				// 色取得
	void SetSize(const MyLib::Vector3 size);		// サイズの設定
	MyLib::Vector3 GetSize(void) const;			// サイズの取得
	MyLib::Vector3 GetVtxMax(void) const;			// 頂点の最大値取得
	MyLib::Vector3 GetVtxMin(void) const;			// 頂点の最小値取得
	int GetIdxXFile(void) const;				// Xファイルのインデックス取得
	bool GetUseShadow(void) const;				// 影を使っているかどうか

	float GetHeight(MyLib::Vector3 pos, bool &bLand);	// 高さ取得

	void Kill(void);
	static CObjectX *Create();
	static CObjectX *Create(const char *pFileName);
	static CObjectX *Create(const char *pFileName, MyLib::Vector3 pos, MyLib::Vector3 rot = MyLib::Vector3(0.0f, 0.0f, 0.0f), bool bShadow = false);
	static CObjectX *Create(int nIdxXFile, MyLib::Vector3 pos, MyLib::Vector3 rot = MyLib::Vector3(0.0f, 0.0f, 0.0f), bool bShadow = false);
	CObjectX *GetObjectX(void);
	static int GetNumAll(void);
protected:

private:
	D3DXMATRIX	m_mtxWorld;	// ワールドマトリックス
	MyLib::Vector3 m_scale;	// スケール
	D3DXCOLOR m_col;		// 色
	MyLib::Vector3 m_fSize;	// サイズ
	bool m_bShadow;			// 影を使うかどうか
	int m_nIdxTexure;		// テクスチャのインデックス番号
	int m_nIdxXFile;		// Xファイルのインデックス番号
	static int m_nNumAll;	// 総数
	CShadow *m_pShadow;		// 影の情報
};



#endif