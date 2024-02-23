//=============================================================================
// 
//  オブジェクトヘッダー [object.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _OBJECT_H_
#define _OBJECT_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"
#include "renderer.h"
#include "texture.h"

class CObject2D;
class CObject3D;
class CObject3DMesh;
class CObjectX;
class CObjectChara;
class CEffect3D;

//==========================================================================
// クラス定義
//==========================================================================
// オブジェクトクラス定義
class CObject
{
public:

	// 列挙型定義
	enum TYPE
	{
		TYPE_NONE = 0,		// 無し
		TYPE_PLAYER,		// プレイヤー
		TYPE_ENEMY,			// 敵
		TYPE_BOSS,			// ボス
		TYPE_BULLET,		// 弾
		TYPE_EXPLOSION,		// 爆発
		TYPE_BG,			// 背景
		TYPE_EFFECT2D,		// 2Dエフェクト
		TYPE_EFFECT3D,		// 3Dエフェクト
		TYPE_SCORE,			// スコア
		TYPE_TIMER,			// タイマー
		TYPE_BLOCK,			// ブロック
		TYPE_ITEM,			// アイテム
		TYPE_FIELD,			// フィールド
		TYPE_HPGAUGE,		// HPゲージ
		TYPE_SHADOW,		// 影
		TYPE_MAP,			// マップ
		TYPE_XFILE,			// Xファイル
		TYPE_STAGE,			// Xファイル
		TYPE_MODEL,			// モデル
		TYPE_EDIT,			// エディット
		TYPE_MESHFIELD,		// メッシュフィールド
		TYPE_ELEVATION,		// 起伏フィールド
		TYPE_ELEVATION_OUTSIDE,		// 起伏フィールド
		TYPE_WATERFIELD,	// 水のフィールド
		TYPE_MAGMA,		// マグマフィールド
		TYPE_MESHWALL,		// メッシュウォール
		TYPE_MESHCYLINDER,	// メッシュシリンダー
		TYPE_MESHDOME,		// メッシュドーム
		TYPE_MESHDONUTS,	// メッシュドーナツ
		TYPE_MESHSPHERE,	// メッシュスフィア
		TYPE_FOG,			// フォグ
		TYPE_BALLAST,		// 瓦礫
		TYPE_FADE,			// フェード
		TYPE_NUMBER,		// 数字
		TYPE_OBJECT2D,		// オブジェクト2D
		TYPE_OBJECT3D,		// オブジェクト3D
		TYPE_OBJECTMESH,		// オブジェクト3D
		TYPE_OBJECTX,		// オブジェクトX
		TYPE_OBJECTBILLBOARD,		// オブジェクトビルボード
		TYPE_MAX
	};

	CObject(int nPriority = mylib_const::PRIORITY_DEFAULT);
	virtual ~CObject();

	// 派生クラスでオーバーライドされた関数が絶対必要(純粋仮想関数)
	virtual HRESULT Init() = 0;
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void SetVtx();
	virtual void SetPosition(const MyLib::Vector3& pos);		// 位置設定
	virtual MyLib::Vector3 GetPosition() const;			// 位置取得
	virtual void SetOldPosition(const MyLib::Vector3& posOld);	// 過去の位置設定
	virtual MyLib::Vector3 GetOldPosition() const;			// 過去の位置取得
	virtual void SetOriginPosition(const MyLib::Vector3& pos);	// 元の位置設定
	virtual MyLib::Vector3 GetOriginPosition() const;		// 元の位置取得
	virtual void SetMove(const MyLib::Vector3& move);			// 移動量設定
	virtual MyLib::Vector3 GetMove() const;				// 移動量取得
	virtual void SetRotation(const MyLib::Vector3& rot);		// 向き設定
	virtual MyLib::Vector3 GetRotation() const;			// 向き取得
	virtual void SetOriginRotation(const MyLib::Vector3& rot);	// 元の向き設定
	virtual MyLib::Vector3 GetOriginRotation() const;		// 元の向き取得

	// 変更処理
	virtual void AddPosition(const MyLib::Vector3& pos);		// 位置設定

	float ScreenZ() const;
	static bool ZSort(const CObject *obj1, const CObject *obj2);	// Zソートの比較関数
	int GetPriority() { return m_nPriority; }	// 優先順位取得
	void SetEnableDisp(bool bDisp);		// 描画設定
	bool IsDisp() { return m_bDisp; }					// 描画状況取得
	void SetEnableHitstopMove() { m_bHitstopMove = true; }	// ヒットストップ中に動くフラグ有効

	int SetEffectParent(CEffect3D *pEffect3D);	// エフェクト設定
	CEffect3D *GetEffectParent(int nIdx);	// 親設定したエフェクト取得
	int GetEffectParentNum();			// 親設定したエフェクトの数取得
	void ReleaseEffect(int nIdx);			// エフェクトの解放
	void UninitEffect();				// エフェクトの解放

	static void ReleaseAll();
	static void UpdateAll();
	static void DrawAll();
	static const int GetNumAll() { return m_nNumAll; }
	static const int GetNumPriorityAll(int nPriority) { return m_nNumPriorityAll[nPriority]; }

	static CObject *GetTop(int nPriority);	// 先頭のオブジェクト取得
	CObject *GetNext();					// 次のオブジェクト取得
	bool IsDeath();						// 死亡の判定
	void SetType(const TYPE type);
	TYPE GetType() const;
	virtual CObject *GetObject();
	virtual CObject2D *GetObject2D();
	virtual CObject3D *GetObject3D();
	virtual CObject3DMesh *GetObject3DMesh();
	virtual CObjectX *GetObjectX();
	virtual CObjectChara *GetObjectChara();

protected:

	void Release();	// 開放処理
	void Death();	// 死亡処理

private:

	static void DrawNone(int nPriority);	// 通常描画
	static void DrawZSort(int nPriority);	// Zソート描画

	MyLib::Vector3 m_pos;			// 位置
	MyLib::Vector3 m_posOld;		// 前回の位置
	MyLib::Vector3 m_posOrigin;	// 元の位置
	MyLib::Vector3 m_rot;			// 向き
	MyLib::Vector3 m_rotOrigin;	// 元の向き
	MyLib::Vector3 m_move;			// 移動量

	int m_nPriority;		// 優先順位
	static int m_nNumAll;	// オブジェクトの総数
	static int m_nNumPriorityAll[mylib_const::PRIORITY_NUM];
	TYPE m_type;			// 種類
	static CObject *m_pTop[mylib_const::PRIORITY_NUM];	// 先頭のオブジェクトへのポインタ
	static CObject *m_pCur[mylib_const::PRIORITY_NUM];	// 最後尾のオブジェクトへのポインタ
	CObject *m_pPrev;	// 前のオブジェクトへのポインタ
	CObject *m_pNext;	// 次のオブジェクトへのポインタ
	bool m_bDeath;		// 死亡フラグ
	bool m_bDisp;		// 描画フラグ
	CEffect3D *m_pEffect[mylib_const::MAX_OBJ];	// エフェクトのポインタ
	int m_nNumEffectParent;	// エフェクトの親設定した数
	bool m_bHitstopMove;		// ヒットストップ時に動くかのフラグ

};

#endif