//=============================================================================
// 
//  マップヘッダー [map.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MAP_H_
#define _MAP_H_	// 二重インクルード防止

#include "main.h"
#include "listmanager.h"

//==========================================================================
// 名前空間
//==========================================================================
namespace MyMap
{
	HRESULT Create(const char *pTextFile);	// 生成処理
	HRESULT ReadTexture(void);	// テクスチャ読み込み処理
	HRESULT ReadXFile(const char *pTextFile);	// もでる読み込み処理
	HRESULT ReadText(const char *pTextFile);	// 外部ファイル読み込み処理
	HRESULT SaveText(void);	// 外部ファイル書き込み処理
	int GetNumModelAll(void);	// 総数取得
	void Release(void);
	void Regist(int nType, MyLib::Vector3 pos, MyLib::Vector3 rot, bool bShadow);
	void ChangeMap(const char* pTextFile);	// マップ切り替え
	std::string GetModelFileName(int nIdx);
};

namespace MyMyMap
{
	// 構造体定義
	struct sMapModel
	{
		int nType;				// 種類
		int nIdxXFile;			// Xファイルのインデックス番号
		MyLib::Vector3 pos;		// 位置
		MyLib::Vector3 rot;		// 向き
		int nShadow;			// 影使うかどうか
	};
};

#endif