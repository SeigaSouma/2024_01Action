//=============================================================================
// 
//  オブジェクト処理 [object.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "object.h"
#include "manager.h"
#include "3D_effect.h"
#include "camera.h"
#include "MyEffekseer.h"
#include "fog.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CObject::m_nNumAll = 0;	// 総数
int CObject::m_nNumPriorityAll[mylib_const::PRIORITY_NUM] = {};
CObject *CObject::m_pTop[mylib_const::PRIORITY_NUM]= {};	// 先頭のオブジェクトへのポインタ
CObject *CObject::m_pCur[mylib_const::PRIORITY_NUM]= {};	// 最後尾のオブジェクトへのポインタ

//==========================================================================
// コンストラクタ
//==========================================================================
CObject::CObject(int nPriority)
{
	if (nPriority < 0 || nPriority >= mylib_const::PRIORITY_NUM)
	{// 範囲外だったら

		return;
	}

	// 値のクリア
	m_pos = 0.0f;		// 位置
	m_posOld = 0.0f;	// 前回の位置
	m_rot = 0.0f;		// 向き
	m_move = 0.0f;		// 移動量
	m_rotOrigin = 0.0f;	// 元の向き

	m_pPrev = nullptr;	// 前のオブジェクトへのポインタ
	m_pNext = nullptr;	// 次のオブジェクトへのポインタ
	m_nPriority = nPriority;	// 優先順位
	m_type = TYPE_NONE;			// 種類
	m_bDeath = false;			// 死亡フラグ
	m_bDisp = true;			// 描画フラグ
	memset(&m_pEffect[0], 0, sizeof(m_pEffect));	// エフェクトのポインタ
	m_nNumEffectParent = 0;		// エフェクトの親設定した数
	m_bHitstopMove = false;		// ヒットストップ時に動くかのフラグ
	m_nNumAll++;				// 総数加算
	m_nNumPriorityAll[nPriority]++;

	// 最後尾を保存
	CObject *pObjCur = m_pCur[nPriority];

	if (pObjCur != nullptr)
	{// 最後尾が存在していたら

		pObjCur->m_pNext = this;				// 最後尾の次に自分自身を代入

		m_pCur[nPriority] = this;				// 最後尾を自分自身に更新
		m_pCur[nPriority]->m_pPrev = pObjCur;	// 最後尾の前に過去の最後尾を代入
	}
	else
	{// 最後尾が無かったら

		m_pCur[nPriority] = this;	// 最後尾を自分自身に更新
	}

	if (m_pTop[nPriority] == nullptr)
	{// 先頭に何もなかったら

		m_pTop[nPriority] = this;	// 先頭に自分自身のポインタを渡す
	}

}

//==========================================================================
// デストラクタ
//==========================================================================
CObject::~CObject()
{

}

//==========================================================================
// 全てのオブジェクトの破棄
//==========================================================================
void CObject::ReleaseAll()
{
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObject = m_pTop[nCntPriority];

		while (pObject != nullptr)
		{// nullptrが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObject->m_pNext;

			if (pObject->m_type != TYPE_NONE)
			{// NONEじゃなければ

				// 終了処理
				pObject->Uninit();
			}

			// 次のオブジェクトを代入
			pObject = pObjNext;
		}

		//*******************************
		// 死亡処理
		//*******************************
		// 先頭を保存
		pObject = m_pTop[nCntPriority];

		while (pObject != nullptr)
		{// nullptrが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObject->m_pNext;

			if (pObject->m_bDeath == true)
			{// 死亡フラグが立っていたら

				// 完全死亡処理
				pObject->Death();
			}
			else
			{
				int n = 0;
			}

			// 次のオブジェクトを代入
			pObject = pObjNext;
		}
	}

	// 優先順位的に消えなかったものを全削除
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObject = m_pTop[nCntPriority];

		while (pObject != nullptr)
		{// nullptrが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObject->m_pNext;

			if (pObject->m_bDeath == true)
			{// 死亡フラグが立っていたら

				// 完全死亡処理
				pObject->Death();
			}
			else
			{
				int n = 0;
			}

			// 次のオブジェクトを代入
			pObject = pObjNext;
		}
	}
}

//==========================================================================
// 全てのオブジェクトの更新処理
//==========================================================================
void CObject::UpdateAll()
{
#if _DEBUG
	// エディットの情報取得
	CEdit *pEdit = CManager::GetInstance()->GetEdit();
#endif

	// ヒットストップ中か
	bool bHitstop = CManager::GetInstance()->IsHitStop();

	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObject = m_pTop[nCntPriority];

		while (pObject != nullptr)
		{// nullptrが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObject->m_pNext;

#if _DEBUG
			if (pObject->m_bDeath == false && pEdit != nullptr && (pObject->m_type == TYPE_EDIT || pObject->m_type == TYPE_XFILE || pObject->m_type == TYPE_ELEVATION) &&
				(pObject->m_bHitstopMove == true || (pObject->m_bHitstopMove == false && bHitstop == false)))
			{// エディット状態だったらエディットのみ更新

				// 更新処理
				pObject->Update();
			}
			else if (pObject->m_bDeath == false && pEdit == nullptr && pObject->m_type != TYPE_NONE &&
				(pObject->m_bHitstopMove == true || (pObject->m_bHitstopMove == false && bHitstop == false)))
			{// エディット状態じゃない && タイプがNONE以外

				// 更新処理
				pObject->Update();
			}
#else
			if (pObject->m_bDeath == false && pObject->m_type != TYPE_NONE &&
				(pObject->m_bHitstopMove == true || (pObject->m_bHitstopMove == false && bHitstop == false)))
			{// タイプがNONE以外

				// 更新処理
				pObject->Update();
			}
#endif

			// 次のオブジェクトを代入
			pObject = pObjNext;
		}

		//*******************************
		// 死亡処理
		//*******************************
		// 先頭を保存
		pObject = m_pTop[nCntPriority];

		while (pObject != nullptr)
		{// nullptrが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObject->m_pNext;

			if (pObject->m_bDeath == true)
			{// 死亡フラグが立っていたら

				// 完全死亡処理
				pObject->Death();
			}

			// 次のオブジェクトを代入
			pObject = pObjNext;
		}
	}
}

//==========================================================================
// 全てのオブジェクトの描画処理
//==========================================================================
void CObject::DrawAll()
{
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		if (nCntPriority == mylib_const::PRIORITY_ZSORT)
		{// Zソート描画
			DrawZSort(nCntPriority);
		}
		else
		{// 通常描画
			DrawNone(nCntPriority);
		}

		if (nCntPriority == 3)
		{
			// エフェクシアの更新兼描画
			CMyEffekseer* pEffekseer = CMyEffekseer::GetInstance();
			if (pEffekseer != nullptr)
			{
				pEffekseer->Update();
			}
		}
	}
}

//==========================================================================
// 通常描画
//==========================================================================
void CObject::DrawNone(int nPriority)
{
	// 先頭を保存
	CObject *pObject = m_pTop[nPriority];

	while (pObject != nullptr)
	{// nullptrが来るまで無限ループ

		// 次のオブジェクトを一時保存
		CObject *pObjNext = pObject->m_pNext;

		if (pObject->m_bDisp == true &&
			pObject->m_bDeath == false &&
			pObject->m_type != TYPE_NONE)
		{// NONEじゃなければ

			// 描画処理
			pObject->Draw();
		}

		// 次のオブジェクトを代入
		pObject = pObjNext;
	}
}

//==========================================================================
// Zソート描画
//==========================================================================
void CObject::DrawZSort(int nPriority)
{
	// 先頭を保存
	CObject *pObject = m_pTop[nPriority];

	// リストコピー
	std::vector<CObject*> pObjectSort;
	while (pObject != nullptr)
	{
		// 次のオブジェクトを一時保存
		CObject *pObjNext = pObject->m_pNext;

		// 要素を末尾に追加
		pObjectSort.push_back(pObject);

		// 次のオブジェクトを代入
		pObject = pObjNext;
	}

	// Zソート
	std::sort(pObjectSort.begin(), pObjectSort.end(), ZSort);

	for (int i = 0; i < (int)pObjectSort.size(); i++)
	{
		if (pObjectSort[i]->m_bDisp == true
			&& pObjectSort[i]->m_bDeath == false
			&& pObjectSort[i]->m_type != TYPE_NONE)
		{// NONEじゃなければ

			// 描画処理
			pObjectSort[i]->Draw();
		}
	}
}

//==========================================================================
// スクリーン座標取得(Z座標)
//==========================================================================
float CObject::ScreenZ() const 
{
	// カメラの座標変換行列を取得
	D3DXMATRIX mtxCamera = CManager::GetInstance()->GetCamera()->GetMtxView();

	// オブジェクト座標をカメラからの座標に変換
	MyLib::Vector3 screenPos;
	D3DXVec3TransformCoord(&screenPos, &m_pos, &mtxCamera);

	// カメラ視点から見たZ座標
	float screenZ = -screenPos.z;

	return screenZ;
}

//==========================================================================
// Zソートの比較関数
//==========================================================================
bool CObject::ZSort(const CObject *obj1, const CObject *obj2)
{
	bool bSort = false;
	if (obj1->ScreenZ() < obj2->ScreenZ())
	{
		bSort = true;
	}

	return bSort;
}

// 描画設定
void CObject::SetEnableDisp(bool bDisp)
{
	m_bDisp = bDisp;
}

//==========================================================================
// エフェクト設定
//==========================================================================
int CObject::SetEffectParent(CEffect3D *pEffect3D)
{
	int nIdx = 0;
	for (int nCntEffect = 0; nCntEffect < mylib_const::MAX_OBJ; nCntEffect++)
	{
		if (m_pEffect[nCntEffect] != nullptr)
		{// nullptrじゃなかったら
			continue;
		}

		// エフェクトのポインタを渡す
		m_pEffect[nCntEffect] = pEffect3D;
		nIdx = nCntEffect;

		// エフェクトの親設定した数加算
		m_nNumEffectParent++;
		break;
	}

	return nIdx;
}

//==========================================================================
// 親設定したエフェクトの数取得
//==========================================================================
int CObject::GetEffectParentNum()
{
	return m_nNumEffectParent;
}

//==========================================================================
// 親設定したエフェクト取得
//==========================================================================
CEffect3D *CObject::GetEffectParent(int nIdx)
{
	return m_pEffect[nIdx];
}

//==========================================================================
// エフェクトの解放
//==========================================================================
void CObject::ReleaseEffect(int nIdx)
{
	// エフェクトnullptrにする
	m_pEffect[nIdx] = nullptr;

	// エフェクトの親設定した数減算
	m_nNumEffectParent--;
}

//==========================================================================
// エフェクトの解放
//==========================================================================
void CObject::UninitEffect()
{
	for (int nCntEffect = 0; nCntEffect < mylib_const::MAX_OBJ; nCntEffect++)
	{
		if (m_pEffect[nCntEffect] == nullptr)
		{// nullptrだったら
			continue;
		}
		m_pEffect[nCntEffect]->UninitParent();
		m_pEffect[nCntEffect] = nullptr;
	}
}

//==========================================================================
// オブジェクトの死亡判定
//==========================================================================
void CObject::Release()
{
	// エフェクトの解放
	UninitEffect();

	// 死亡フラグを立てる
	m_bDeath = true;
}

//==========================================================================
// オブジェクトの破棄・死亡処理
//==========================================================================
void CObject::Death()
{
	// 今回の番号保存
	int nPriority = m_nPriority;

	// オブジェクトを保存
	CObject *pObject = this;
	CObject *pObjNext = pObject->m_pNext;
	CObject *pObjPrev = pObject->m_pPrev;

	if (pObject == nullptr)
	{// nullptrだったら
		return;
	}

	// 前のオブジェクトと次のオブジェクトを繋ぐ
	if (pObjNext == nullptr)
	{// 自分が最後尾の時

		if (pObjPrev != nullptr)
		{// 前のオブジェクトがあれば

			// 前のオブジェクトの次をnullptrにする
			pObjPrev->m_pNext = nullptr;

			// 最後尾を自分の前のオブジェクトにする
			m_pCur[nPriority] = pObjPrev;
		}
		else
		{// 前のオブジェクトがない時

			// 最後尾をnullptrにする
			m_pCur[nPriority] = nullptr;
		}
	}
	else
	{// 最後尾じゃないとき

		if (pObjPrev != nullptr)
		{// 自分が先頭じゃない時

			// 前のオブジェクトの次を, 自分の次にする
			pObjPrev->m_pNext = pObject->m_pNext;
		}

	}

	if (pObjPrev == nullptr)
	{// 自分が先頭の時

		if (pObjNext != nullptr)
		{// 次のオブジェクトがある時

			// 次のオブジェクトの前をnullptrにする
			pObjNext->m_pPrev = nullptr;

			// 先頭を自分の次のオブジェクトにする
			m_pTop[nPriority] = pObjNext;
		}
		else
		{// 次のオブジェクトがない時

			// 先頭をnullptrにする
			m_pTop[nPriority] = nullptr;
		}
	}
	else
	{// 先頭じゃないとき

		if (pObjNext != nullptr)
		{// 自分が最後尾じゃない時

			// 次のオブジェクトの前を, 自分の前にする
			pObjNext->m_pPrev = pObject->m_pPrev;
		}
	}

	//if (m_pReleaseNext == pObject)
	//{// 本来次に消すオブジェクトと今回消すオブジェクトが同じとき
	//	m_pReleaseNext = pObjNext;
	//}

	// メモリの開放
	delete pObject;
	pObject = nullptr;

	// 総数を減らす
	m_nNumAll--;
	m_nNumPriorityAll[nPriority]--;
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CObject::SetVtx()
{

}

//==========================================================================
// 位置設定
//==========================================================================
void CObject::SetPosition(const MyLib::Vector3& pos)
{
	m_pos = pos;
}

//==========================================================================
// 位置加算
//==========================================================================
void CObject::AddPosition(const MyLib::Vector3& pos)
{
	m_pos += pos;
}

//==========================================================================
// 位置取得
//==========================================================================
MyLib::Vector3 CObject::GetPosition() const
{
	return m_pos;
}

//==========================================================================
// 位置設定
//==========================================================================
void CObject::SetOldPosition(const MyLib::Vector3& posOld)
{
	m_posOld = posOld;
}

//==========================================================================
// 位置取得
//==========================================================================
MyLib::Vector3 CObject::GetOldPosition() const
{
	return m_posOld;
}

//==========================================================================
// 元の位置設定
//==========================================================================
void CObject::SetOriginPosition(const MyLib::Vector3& pos)
{
	m_posOrigin = pos;
}

//==========================================================================
// 元の位置取得
//==========================================================================
MyLib::Vector3 CObject::GetOriginPosition() const
{
	return m_posOrigin;
}

//==========================================================================
// 移動量設定
//==========================================================================
void CObject::SetMove(const MyLib::Vector3& move)
{
	m_move = move;
}

//==========================================================================
// 移動量取得
//==========================================================================
MyLib::Vector3 CObject::GetMove() const
{
	return m_move;
}

//==========================================================================
// 向き設定
//==========================================================================
void CObject::SetRotation(const MyLib::Vector3& rot)
{
	m_rot = rot;
}

//==========================================================================
// 向き取得
//==========================================================================
MyLib::Vector3 CObject::GetRotation() const
{
	return m_rot;
}

//==========================================================================
// 元の向き設定
//==========================================================================
void CObject::SetOriginRotation(const MyLib::Vector3& rot)
{
	m_rotOrigin = rot;
}

//==========================================================================
// 元の向き取得
//==========================================================================
MyLib::Vector3 CObject::GetOriginRotation() const
{
	return m_rotOrigin;
}

//==========================================================================
// 種類の設定
//==========================================================================
void CObject::SetType(const TYPE type)
{
	m_type = type;
}

//==========================================================================
// 種類の取得
//==========================================================================
CObject::TYPE CObject::GetType() const
{
	return m_type;
}

//==========================================================================
// オブジェクトの取得
//==========================================================================
CObject *CObject::GetObject()
{
	return this;
}

//==========================================================================
// 先頭のオブジェクト取得
//==========================================================================
CObject *CObject::GetTop(int nPriority)
{
	return m_pTop[nPriority];
}

//==========================================================================
// 次のオブジェクト取得
//==========================================================================
CObject *CObject::GetNext()
{
	return this->m_pNext;
}

//==========================================================================
// 死亡の判定
//==========================================================================
bool CObject::IsDeath()
{
	return m_bDeath;
}

//==========================================================================
// オブジェクト2Dオブジェクトの取得
//==========================================================================
CObject2D *CObject::GetObject2D()
{
	return nullptr;
}

//==========================================================================
// オブジェクト3Dオブジェクトの取得
//==========================================================================
CObject3D *CObject::GetObject3D()
{
	return nullptr;
}

//==========================================================================
// オブジェクト3Dメッシュオブジェクトの取得
//==========================================================================
CObject3DMesh *CObject::GetObject3DMesh()
{
	return nullptr;
}

//==========================================================================
// オブジェクトXオブジェクトの取得
//==========================================================================
CObjectX *CObject::GetObjectX()
{
	return nullptr;
}

//==========================================================================
// オブジェクトキャラクターオブジェクトの取得
//==========================================================================
CObjectChara *CObject::GetObjectChara()
{
	return nullptr;
}