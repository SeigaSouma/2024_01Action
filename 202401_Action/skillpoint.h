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

	CSkillPoint(int nPriority = 8);
	~CSkillPoint();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPoint(int nValue);	// ポイント設定
	void AddPoint(void);		// ポイント追加
	void AddPoint(int nValue);	// ポイント追加
	void SubPoint(void);		// ポイント削除
	int GetPoint(void) const { return m_nPoint; }		// ポイント取得
	static CSkillPoint* Create(void);

private:

	// メンバ変数
	int m_nPoint;	// ポイント
	CMultiNumber* m_apNumber;	// 数字のオブジェクト
	//LPDIRECT3DTEXTURE9 multi;
};



#endif