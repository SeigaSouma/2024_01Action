//=============================================================================
// 
//  オブジェクトキャラクターヘッダー [objectChara.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _OBJECTCHARA_H_
#define _OBJECTCHARA_H_	// 二重インクルード防止

#include "objectHierarchy.h"
#include "motion.h"

//==========================================================================
// クラス定義
//==========================================================================
// オブジェクトキャラクタークラス定義
class CObjectChara : public CObjectHierarchy
{
public:

	CObjectChara(int nPriority = mylib_const::PRIORITY_DEFAULT);
	~CObjectChara();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Draw(D3DXCOLOR col) override;
	void Draw(float fAlpha) override;

	void SetVelocity(const float fVelocity);	// 移動速度設定
	float GetVelocity(void) const;				// 移動速度取得
	void SetRotDest(const float fRotDest);		// 目標の向き設定
	float GetRotDest(void) const;				// 目標の向き取得
	void SetLife(const int nLife);				// 体力設定
	int GetLife(void) const;					// 体力取得
	int GetLifeOrigin(void) const;				// 元の体力取得
	int GetMotionStartIdx(void) const;			// モーション開始のインデックス番号取得
	int GetAddScoreValue(void) const;			// スコア加算量取得

	HRESULT SetCharacter(const std::string pTextFile) override;	// キャラクター設定
	CObjectChara *GetObjectChara(void);
	static CObjectChara *Create(const std::string pTextFile);

protected:

	void LoadObjectData(FILE* pFile, const char* pComment) override;	// オブジェクト毎のデータ読み込み
	void LoadPartsData(FILE* pFile, const char* pComment, int *pCntParts) override;		// パーツ毎のデータ読み込み
	void BindObjectData(int nCntData) override;							// オブジェクト毎のデータ割り当て
	virtual void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK);	// 攻撃時処理
	virtual void AttackInDicision(CMotion::AttackInfo ATKInfo, int nCntATK);			// 攻撃判定中処理
	void ChangeMotion(const char* pMotionFile);	// モーションファイル変更
	CMotion* GetMotion(void);	// モーションオブジェクト取得
private:

	// メンバ関数
	void MotionInProgressAction(void);	// モーション中の行動処理

	// メンバ変数
	float m_fVelocity;			// 移動速度
	float m_fRotDest;			// 目標の向き
	int m_nLife;				// 体力
	int m_nLifeOrigin;			// 元の体力
	int m_nMotionStartIdx;		// モーション開始のインデックス番号
	int m_nAddScore;			// スコア加算量
	CMotion *m_pMotion;			// モーションの情報
};



#endif