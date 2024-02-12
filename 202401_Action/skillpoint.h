//=============================================================================
// 
//  スキルポイントヘッダー [skillpoint.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLPOINT_H_
#define _SKILLPOINT_H_	// 二重インクルード防止

class CMultiNumber;

#include "object2D.h"
//#include "object2D_Multi.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルポイントクラス定義
class CSkillPoint : public CObject2D
{
public:

	CSkillPoint(int nPriority = 9);
	~CSkillPoint();

	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	void Kill();

	void SetPoint(int nValue);	// ポイント設定
	int GetPoint() const { return m_nPoint; }		// ポイント取得
	void AddPoint();		// ポイント追加
	void AddPoint(int nValue);	// ポイント追加
	void SubPoint();		// ポイント削除
	void SubPoint(int nValue);	// ポイント削除
	static CSkillPoint* Create();

private:

	// メンバ変数
	int m_nPoint;	// ポイント
	CMultiNumber* m_apNumber;	// 数字のオブジェクト
};



#endif