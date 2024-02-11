//=============================================================================
// 
//  敵の拠点エディターヘッダー [edit_enemybase.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EDIT_ENEMYBASE_H_
#define _EDIT_ENEMYBASE_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"
#include "camera.h"

class CEnemy;

//==========================================================================
// クラス定義
//==========================================================================
// カメラ追従種類変更エディットクラス定義
class CEditEnemyBase
{
public:

	CEditEnemyBase();
	~CEditEnemyBase();

	// オーバーライドされた関数
	HRESULT Init();
	HRESULT Init(const char *pFileName);
	void Uninit();
	void Update();
	void Draw();

	void Release();
	static CEditEnemyBase *Create();	// 生成処理

private:

	void Control(MyLib::Vector3 &pos);	// 操作
	void Grab();	// 掴み
	void Delete();	// 削除
	void ChangeType();

	int m_nIdxMapPoint;			// マップポイントのインデックス番号
	float m_fPointRatio;		// 移動割合
	float m_fMoveValue;			// 移動量
	MyLib::Vector3 m_pos;			// 位置
	MyLib::Vector3 m_rot;			// 位置
	int m_nEnemyType;			// 敵の種類
	int m_nStage;				// ステージ
	int m_nRush;				// ラッシュ用かどうか
	
	CEnemy *m_pEnemy[mylib_const::MAX_PATTEN_ENEMY];	// 敵へのポインタ
	CObjectX *m_apObjX;	// オブジェクトX
};



#endif