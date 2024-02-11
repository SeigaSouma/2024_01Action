//=============================================================================
// 
//  Xファイルヘッダー [Xload.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _XLOAD_H_
#define _XLOAD_H_	// 二重インクルード防止

#include "object.h"

class CObjectX;

//==========================================================================
// クラス定義
//==========================================================================
// Xファイルクラス定義
class CXLoad
{
public:

	// マクロ定義
#define MAX_MAT				(512)		// マテリアルサイズ

	struct SXFile
	{
		MyLib::Vector3 vtxMin;					// モデルの最小値
		MyLib::Vector3	vtxMax;					// モデルの最大値
		BYTE *pVtxBuff;							// 頂点バッファのポインタ
		LPD3DXMESH pMesh;						// メッシュ(頂点情報)へのポインタ
		LPD3DXBUFFER pBuffMat;					// マテリアルへのポインタ
		DWORD dwNumMat;							// マテリアルの数
		D3DXMATERIAL pMatData[MAX_MAT];			// マテリアルのデータ
		int nVtxNum;							// 頂点数
		int nFaceNum;							// 面の数
		MyLib::Vector3 *pVtxPos;				// 頂点座標
		float fMaxVtxDistance;					// 頂点間の最大距離
		int *nIdxTexture;						// テクスチャのインデックス番号
		std::string filename;					// ファイル名
		int nFileNameLen;						// ファイル名の文字数
	};

	CXLoad();
	~CXLoad();

	HRESULT Init();
	void Uninit();
	int XLoad(std::string file);

	SXFile *GetMyObject(int nIdx);
	int GetNumAll();		// 総数

	static CXLoad* Create();
	static CXLoad* GetInstance() { return m_pXX; }
protected:

private:
	HRESULT Load(std::string file);
	static void Unload();

	std::vector<SXFile> m_XFileInfo;	// Xファイルの情報
	static CXLoad* m_pXX;	// 自身のポインタ
};



#endif