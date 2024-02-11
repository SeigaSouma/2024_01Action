//=============================================================================
// 
//  エディットヘッダー [edit.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EDIT_H_
#define _EDIT_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

class CObjectX;

//==========================================================================
// クラス定義
//==========================================================================
// エディットクラス定義
class CEdit : public CObject
{
public:

	CEdit(int nPriority = 0);
	~CEdit();

	// オーバーライドされた関数
	HRESULT Init();
	HRESULT Init(const char *pFileName);
	void Uninit();
	void Update();
	void Draw();

	static CEdit *Create();
	void Release();
	static int GetNumAll();
protected:

private:
	HRESULT ReadText();
	void Control(CObjectX *pObjX);
	void ChangeType();
	void GrabModel();
	void DeleteModel();

	static CObjectX *m_pObjX;
	MyLib::Vector3 m_posOld;	// 前回の位置
	static int m_nNumAll;	// 総数
	static int m_nType;		// タイプ
	static bool m_bShadow;	// 影を使うかどうか
};



#endif