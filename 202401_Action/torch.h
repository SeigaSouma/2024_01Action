//=============================================================================
// 
//  松明ヘッダー [torch.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TORCH_H_
#define _TORCH_H_	// 二重インクルード防止

#include "objectX.h"
#include "listmanager.h"

//==========================================================================
// クラス定義
//==========================================================================
// 松明クラス
class CTorch : public CObjectX
{
public:

	enum TYPE
	{
		TYPE_WALL = 0,	// 壁掛け
		TYPE_STAND,		// 置き型
		TYPE_MAX
	};

	CTorch(int nPriority = 3);
	~CTorch();

	// オーバーライドされた関数
	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override {}

	void Kill(void);	// 削除

	// 静的関数
	static CTorch *Create(TYPE type, const MyLib::Vector3& pos, const MyLib::Vector3& rot);
	static void SetTorch(void);

	static CListManager<CTorch> GetList() { return m_List; }

protected:
	int m_nCntFire;

private:

	//=============================
	// メンバ関数
	//=============================
	static HRESULT ReadXFile(const char* pTextFile);	// モデル読み込み処理

	//=============================
	// メンバ変数
	//=============================
	static std::vector<std::string> ModelFile;		// モデルファイル名
	static CListManager<CTorch> m_List;		// リスト

};

class CTorchStand : public CTorch
{
public:
	CTorchStand() {}

	// オーバーライドされた関数
	virtual HRESULT Init(void) override;
	virtual void Update(void) override;
};

class CTorchWall : public CTorch
{
public:
	CTorchWall() {}

	// オーバーライドされた関数
	virtual HRESULT Init(void) override;
	virtual void Update(void) override;
};

#endif