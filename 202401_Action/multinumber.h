//=============================================================================
// 
//  数字ヘッダー [multinumber.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MULTINUMBER_H_
#define _MULTINUMBER_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"
#include "number.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CNumber;

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CMultiNumber
{
public:

	// 寄せ種類
	enum AlignmentType
	{
		ALIGNMENT_LEFT = 0,	// 左寄せ
		ALIGNMENT_RIGHT,	// 右寄せ
		ALIGNMENT_MAX
	};

	CMultiNumber(int nPriority = 8);
	~CMultiNumber();

	static CMultiNumber *Create(MyLib::Vector3 pos, D3DXVECTOR2 size, int nNum, CNumber::EObjectType objtype, bool bDigitDraw = false, int nPriority = 8);
	static CMultiNumber *Create(MyLib::Vector3 pos, D3DXVECTOR2 size, int nNum, CNumber::EObjectType objtype, const char *pTextureFile, bool bDigitDraw = true, int nPriority = 8);

	// メンバ関数
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	void AddNumber(int nValue);
	void SetValue();		// 値の設定処理
	void SetValue(int nValue);	// 値の設定処理(オーバーロード)
	int GetValue();			// 値の取得処理
	void SetPosition(const MyLib::Vector3 pos);	// 位置設定
	MyLib::Vector3 GetPosition() const;		// 位置取得
	void SetOriginPosition(const MyLib::Vector3 pos);	// 位置設定
	MyLib::Vector3 GetOriginPosition() const;		// 位置取得
	void SetRotation(const MyLib::Vector3 pos);	// 位置設定
	MyLib::Vector3 GetRotation() const;		// 位置取得
	void SetColor(const D3DXCOLOR col);			// 色設定
	D3DXCOLOR GetColor() const;				// 色取得
	void Release();	// 開放処理
	void SetSize(const D3DXVECTOR2 size);		// サイズ設定
	D3DXVECTOR2 GetSize() const;			// サイズ取得
	void SetSizeOrigin(const D3DXVECTOR2 size);		// サイズ設定
	D3DXVECTOR2 GetSizeOrigin() const;			// サイズ取得
	void SetKerning(float kerning);	// 文字間隔設定
	void SetAlignmentType(AlignmentType type) { m_Alignment = type; }

private:

	void SettingDisp();

	// メンバ変数
	MyLib::Vector3 m_pos;			// 位置
	MyLib::Vector3 m_posOrigin;		// 位置
	MyLib::Vector3 m_rot;			// 向き
	D3DXCOLOR m_col;				// 色
	D3DXVECTOR2 m_size;				// 数字のサイズ
	D3DXVECTOR2 m_sizeOrigin;		// 数字のサイズ
	int m_nNum;						// 数字
	int m_nNumNumber;				// 数字の数
	int m_nTexIdx;					// テクスチャのインデックス番号
	int m_nPriority;				// 優先順位
	bool m_bDigitDraw;				// 桁数描画
	float m_fKerning;				// 文字間隔
	CNumber **m_ppMultiNumber;		// 数字のオブジェクト
	CNumber::EObjectType m_objType;	// オブジェクトの種類
	AlignmentType m_Alignment;		// 揃え

};

#endif