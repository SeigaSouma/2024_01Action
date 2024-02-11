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
	HRESULT Init();
	HRESULT Init(const char *pFileName);
	HRESULT Init(int nIdxXFile);
	void Uninit();
	void Update();
	void Draw();
	void Draw(D3DXCOLOR col);
	void Draw(float fAlpha);
	void BindTexture(int *nIdx);
	void BindXData(int nIdxXFile);

	void SetWorldMtx(const D3DXMATRIX mtx);		// マトリックス設定
	D3DXMATRIX GetWorldMtx() const;			// マトリックス取得
	void SetScale(const MyLib::Vector3 scale);		// スケール設定
	MyLib::Vector3 GetScale() const;			// スケール取得
	void SetColor(const D3DXCOLOR col);			// 色設定
	D3DXCOLOR GetColor() const;				// 色取得
	void SetSize(const MyLib::Vector3 size);		// サイズの設定
	MyLib::Vector3 GetSize() const;			// サイズの取得
	MyLib::Vector3 GetVtxMax() const;			// 頂点の最大値取得
	MyLib::Vector3 GetVtxMin() const;			// 頂点の最小値取得
	int GetIdxXFile() const;				// Xファイルのインデックス取得
	bool GetUseShadow() const;				// 影を使っているかどうか

	float GetHeight(MyLib::Vector3 pos, bool &bLand);	// 高さ取得

	void Kill();
	static CObjectX *Create();
	static CObjectX *Create(const char *pFileName);
	static CObjectX *Create(const char *pFileName, const MyLib::Vector3& pos, const MyLib::Vector3& rot = 0.0f, bool bShadow = false);
	static CObjectX *Create(int nIdxXFile, const MyLib::Vector3& pos, const MyLib::Vector3& rot = 0.0f, bool bShadow = false);
	CObjectX *GetObjectX();
	static int GetNumAll();
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