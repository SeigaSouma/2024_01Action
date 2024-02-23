//=============================================================================
// 
//  戦闘開始ヘッダー [battlestart.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BATTLESTART_H_
#define _BATTLESTART_H_	// 二重インクルード防止

#include "object2d.h"

//==========================================================================
// クラス定義
//==========================================================================
// 戦闘開始クラス
class CBattleStart : public CObject2D
{
public:

	CBattleStart(int nPriority = 7);
	~CBattleStart();


	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();

	static CBattleStart* Create(const MyLib::Vector3& pos);

private:

	//=============================
	// 列挙型定義
	//=============================
	enum VTXTYPE
	{
		VTXTYPE_RIGHTUP = 0,	// 右上
		VTXTYPE_LEFTDW,		// 左下
		VTXTYPE_MAX
	};

	enum STATE
	{
		STATE_GETTOGETHER = 0,	// 集まる
		STATE_CHARGE,	// 溜め
		STATE_STINGS,	// 刺さり
		STATE_SCALE,	// 拡大
		STATE_FADEOUT,
		STATE_MAX
	};

	// 構造体定義
	struct SRockOnMarker
	{
		CObject2D* p2D;	// 2Dオブジェのポインタ
		float distance;		// 間隔
		int Angle;			// 向き
		float distancetimer;	// 距離時間

		SRockOnMarker() : p2D(nullptr), distance(0.0f), Angle(0), distancetimer(0.0f) {}
	};

	//=============================
	// 関数リスト
	//=============================
	typedef void(CBattleStart::*STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// 状態関数のリスト

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	void StateGetTogether();	// 集まる
	void StateCharge();			// 溜め
	void StateStings();	// 刺さる
	void StateScale();	// 拡縮
	void StateFadeOut();	// フェードアウト

	//=============================
	// メンバ変数
	//=============================
	STATE m_state;		// 状態
	float m_fStateTime;	// 状態タイマー
	SRockOnMarker m_RockOnInfo[VTXTYPE_MAX];	// マーカー情報
};


#endif