//=============================================================================
// 
//  観衆ヘッダー [gallery.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _GALLERY_H_
#define _GALLERY_H_	// 二重インクルード防止

#include "object.h"
#include "listmanager.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObjectX;

//==========================================================================
// クラス定義
//==========================================================================
// 観衆クラス定義
class CGallery : public CObject
{
public:
	//=============================
	// 列挙型定義
	//=============================
	// 状態
	enum STATE
	{
		STATE_NONE = 0,	// なにもなし
		STATE_CLEARHEAT,		// クリア盛り上がり
		STATE_COUNTERHEAT,		// カウンター盛り上がり
		STATE_MAX
	};

private:

	//=============================
	// 列挙型定義
	//=============================
	// 色の種類
	enum COLORTYPE
	{
		COLORTYPE_RED = 0,
		COLORTYPE_BLUE,
		COLORTYPE_GREEN,
		COLORTYPE_MAX
	};

	// ポーズ
	enum POSE
	{
		POSE_NONE = 0,
		POSE_RIGHT,
		POSE_LEFT,
		POSE_MAX
	};


	//=============================
	// 構造体定義
	//=============================
	struct sGalleryInfo
	{
		CObjectX* objX;			// オブジェクトXのポインタ
		COLORTYPE colorType;	// 色の種類
		POSE poseType;			// ポーズの種類

		// コンストラクタ
		sGalleryInfo() : objX(nullptr), colorType(COLORTYPE_RED), poseType(POSE_RIGHT) {}
	};

public:

	CGallery(int nPriority = mylib_const::PRIORITY_DEFAULT);
	~CGallery();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void Kill(void);	// 削除
	void SetState(STATE state);	// 状態設定
	static CGallery* Create(const MyLib::Vector3& pos, const MyLib::Vector3& rot);	// 生成
	static CListManager<CGallery> GetList() { return m_List; }
	static void SetGallery(void);
private:

	//=============================
	// 関数リスト
	//=============================
	typedef void(CGallery::* STATE_FUNC)(void);
	static STATE_FUNC m_StateFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	void StateNone() {}
	void StateClearHeat();
	void StateCounterHeat();

	// その他
	void Jump();

	//=============================
	// メンバ変数
	//=============================
	STATE m_State;	// 状態
	float m_fStateTime;	// 状態カウンター
	std::vector<sGalleryInfo> m_GalleryInfo;	// 観衆情報
	static std::vector<int> m_nModelIdx;		// モデルインデックス番号
	static CListManager<CGallery> m_List;		// リスト
};


#endif