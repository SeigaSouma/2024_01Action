//=============================================================================
// 
//  リザルトスコアヘッダー [resultscore.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _RESULTSCORE_H_
#define _RESULTSCORE_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define RESULTSCORE_DIGIT	(7)	// 桁数

//==========================================================================
// 前方宣言
//==========================================================================
class CMultiNumber;

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CResultScore : public CObject
{
public:

	CResultScore(int nPriority = 7);
	~CResultScore();

	// オーバーライドされた関数
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	void SetAllArrival();	// 全ての到着設定
	void SetValue(int nScore);	// 値の設定処理
	static CResultScore *Create();
	bool IsSetToatal() { return m_bSetToatal; }
private:
	
	void CreateToatalScore();	// 最終スコアの生成
	void SetToatalValue();		// 最終スコアの値設定処理
	void MoveToatalScore();		// 最終スコアの移動処理

	int m_nToatalNum;				// 値
	int m_nToatalNumDest;			// 目標の値
	int m_nTexIdx;					// テクスチャのインデックス番号
	int m_nTexIdxNumber;			// 数字テクスチャのインデックス番号
	float m_fToatalPosDest_X;		// 最終スコアの目標位置
	bool m_bArrivalToatal;			// 最終スコアの到着判定
	bool m_bSetToatal;				// 最終スコアの設定判定
	CObject2D *m_pToatalObj2D;		// オブジェクト2Dのオブジェクト
	CMultiNumber *m_pToatalScore;	// 数字のオブジェクト
};



#endif