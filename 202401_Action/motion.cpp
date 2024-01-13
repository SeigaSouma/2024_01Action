//=============================================================================
// 
//  モデル処理 [motion.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "motion.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "model.h"
#include "objectChara.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
std::vector<std::string> CMotion::m_sTextFile = {};		// テキストファイル名
std::vector<CMotion::Info> CMotion::m_aLoadData[MAX_MOTION] = {};	// モーションの情報
std::vector<CMotion::AttackInfo> CMotion::m_aLoadAttackData[MAX_MOTION][MAX_PARTS] = {};	// 攻撃の読み込み情報
int CMotion::m_nNumLoad = 0;								// 読み込んだ数
std::vector<int> CMotion::m_nNumLoadData = {};				// モーション毎のデータ数

//==========================================================================
// コンストラクタ
//==========================================================================
CMotion::CMotion()
{
	// 値のクリア
	m_nNumAll = 0;			// モーションの総数
	m_nType = 0;			// 現在のモーションの種類
	m_nOldType = 0;			// 前回のモーションの種類
	m_bLoop = false;		// ループするかどうか
	m_nPatternKey = 0;		// 何個目のキーか
	m_fCntFrame = 0.0f;		// フレームのカウント
	m_fCntAllFrame = 0.0f;	// 全てのカウント
	m_fMaxAllFrame = 0.0f;	// 全てのカウントの最大値
	m_fSlowFactor = 0.0f;	// 遅延係数
	m_bFinish = false;		// 終了したかどうか
	m_pObjChara = NULL;		// オブジェクトのポインタ
	m_ppModel = NULL;		// モデルのポインタ
	m_nNumModel = 0;		// モデルの総数
	m_nNumMotion = 0;		// モーションの総数

	for (int nCntMotion = 0; nCntMotion < MAX_MOTION; nCntMotion++)
	{
		m_aInfo[nCntMotion].nNumKey = 0;			// キーの数
		m_aInfo[nCntMotion].nLoop = 0;				// ループ判定
		m_aInfo[nCntMotion].nMove = 0;				// 移動判定
		m_aInfo[nCntMotion].nNumAttackInfo = 0;		// 攻撃情報の数

		for (int nCntKey = 0; nCntKey < MAX_KEY; nCntKey++)
		{
			m_aInfo[nCntMotion].aKey[nCntKey].nFrame = 0;

			for (int nCntParts = 0; nCntParts < MAX_PARTS; nCntParts++)
			{
				m_aInfo[nCntMotion].aKey[nCntKey].aParts[nCntParts].rot = MyLib::Vector3(0.0f, 0.0f, 0.0f);		// 向き
				m_aInfo[nCntMotion].aKey[nCntKey].aParts[nCntParts].rotDest = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 目標の向き
				m_aInfo[nCntMotion].aKey[nCntKey].aParts[nCntParts].pos = MyLib::Vector3(0.0f, 0.0f, 0.0f);		// 位置
				m_aInfo[nCntMotion].aKey[nCntKey].aParts[nCntParts].posDest = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 目標の位置
				m_aInfo[nCntMotion].aKey[nCntKey].aParts[nCntParts].posOrigin = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 位置の原点
			}
		}

		for (int nCntParts = 0; nCntParts < MAX_PARTS; nCntParts++)
		{
			m_aInfo[nCntMotion].AttackInfo[nCntParts] = NULL;	// 攻撃情報
		}
	}

	for (int nCntParts = 0; nCntParts < MAX_PARTS; nCntParts++)
	{
		// 過去の情報
		aPartsOld[nCntParts].rot = MyLib::Vector3(0.0f, 0.0f, 0.0f);		// 向き
		aPartsOld[nCntParts].rotDest = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 目標の向き
		aPartsOld[nCntParts].pos = MyLib::Vector3(0.0f, 0.0f, 0.0f);		// 位置
		aPartsOld[nCntParts].posDest = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 目標の位置
		aPartsOld[nCntParts].posOrigin = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 位置の原点
	}

	m_nNumAll = 0;			// モーションの総数
	m_nType = 0;			// 現在のモーションの種類
	m_nOldType = 0;			// 前回のモーションの種類
	m_bLoop = false;		// ループするかどうか
	m_nPatternKey = 0;		// 何個目のキーか
	m_fSlowFactor = 0.0f;	// 遅延係数
	m_bFinish = false;		// 終了したかどうか
	m_pObjChara = NULL;		// オブジェクトのポインタ
	m_ppModel = NULL;		// モデルのポインタ
	m_nNumModel = 0;		// モデルの総数
	m_nNumMotion = 0;		// モーションの総数

}

//==========================================================================
// デストラクタ
//==========================================================================
CMotion::~CMotion()
{

}


//==========================================================================
// 生成処理
//==========================================================================
CMotion *CMotion::Create(void)
{
	// 生成用のオブジェクト
	CMotion *pMotion = NULL;

	if (pMotion == NULL)
	{// NULLだったら

		// メモリの確保
		pMotion = DEBUG_NEW CMotion;

		if (pMotion != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pMotion->Init();
		}

		return pMotion;
	}

	return NULL;
}


//==========================================================================
// 生成処理
//==========================================================================
CMotion *CMotion::Create(const std::string pTextFile)
{
	// 生成用のオブジェクト
	CMotion *pMotion = NULL;

	if (pMotion == NULL)
	{// NULLだったら

		// メモリの確保
		pMotion = DEBUG_NEW CMotion;

		if (pMotion != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pMotion->Init();
			pMotion->ReadText(pTextFile);
		}

		return pMotion;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CMotion::Init(void)
{
	m_nNumAll = 0;			// モーションの総数
	m_nType = 0;			// 現在のモーションの種類
	m_bLoop = false;		// ループするかどうか
	m_nPatternKey = 0;		// 何個目のキーか
	m_fCntFrame = 0.0f;		// フレームのカウント
	m_fCntAllFrame = 0.0f;	// 全てのカウント
	m_fMaxAllFrame = 0.0f;	// 全てのカウントの最大値
	m_bFinish = true;		// 終了したかどうか
	m_ppModel = NULL;		// モデルのポインタ
	m_nNumModel = 0;		// モデルの総数
	m_fSlowFactor = 1.0f;	// 遅延係数

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CMotion::Uninit(void)
{
	// 攻撃情報の破棄
	for (int nCntMotion = 0; nCntMotion < MAX_MOTION; nCntMotion++)
	{
		for (int nCntAttack = 0; nCntAttack < MAX_PARTS; nCntAttack++)
		{
			if (m_aInfo[nCntMotion].AttackInfo[nCntAttack] != NULL)
			{
				delete m_aInfo[nCntMotion].AttackInfo[nCntAttack];
				m_aInfo[nCntMotion].AttackInfo[nCntAttack] = NULL;
			}
		}
	}
}

//==========================================================================
// モーション情報の登録
//==========================================================================
void CMotion::SetInfo(Info info)
{
	// コピー
	memcpy(m_aInfo[m_nNumAll].aKey, info.aKey, sizeof(info.aKey));

	m_aInfo[m_nNumAll].nLoop = info.nLoop;
	m_aInfo[m_nNumAll].nMove = info.nMove;
	m_aInfo[m_nNumAll].nNumKey = info.nNumKey;

	for (int nCntKey = 0; nCntKey < m_aInfo[m_nNumAll].nNumKey; nCntKey++)
	{
		int nBeforeCnt = nCntKey - 1;
		if (nBeforeCnt <= 0)
		{
			nBeforeCnt = 0;
		}

		float PosX = m_aInfo[m_nNumAll].aKey[nBeforeCnt].aParts[0].pos.x;
		float PosZ = m_aInfo[m_nNumAll].aKey[nBeforeCnt].aParts[0].pos.z;

		// 向きを求める
		m_aInfo[m_nNumAll].aKey[nCntKey].fRotMove = atan2f(
			(PosX - m_aInfo[m_nNumAll].aKey[nCntKey].aParts[0].pos.x),
			(PosZ - m_aInfo[m_nNumAll].aKey[nCntKey].aParts[0].pos.z));
	}

	// モーションの総数加算
	m_nNumAll++;
}

//==========================================================================
// 攻撃情報の登録
//==========================================================================
void CMotion::SetAttackInfo(AttackInfo info)
{
	// メモリ確保
	m_aInfo[m_nNumAll].AttackInfo[m_aInfo[m_nNumAll].nNumAttackInfo] = DEBUG_NEW AttackInfo;

	if (m_aInfo[m_nNumAll].AttackInfo[m_aInfo[m_nNumAll].nNumAttackInfo] != NULL)
	{// NULLじゃなければ

		// 情報渡す
		*m_aInfo[m_nNumAll].AttackInfo[m_aInfo[m_nNumAll].nNumAttackInfo] = info;

		// 攻撃情報の総数加算
		m_aInfo[m_nNumAll].nNumAttackInfo++;
	}
}

//==========================================================================
// モーションをするモデルの登録
//==========================================================================
void CMotion::SetModel(CModel **pModel, int nNumModel, CObjectChara *pObjChara)
{
	// モデルのポインタを渡す
	m_ppModel = pModel;

	// モデルの総数
	m_nNumModel = nNumModel;

	// オブジェクトのポインタを渡す
	m_pObjChara = pObjChara;
}

//==========================================================================
// ポーズのリセット
//==========================================================================
void CMotion::ResetPose(int nType)
{

	int nStartIdx = m_pObjChara->GetMotionStartIdx();
	for (int nCntParts = nStartIdx; nCntParts < m_nNumModel + nStartIdx + 1; nCntParts++)
	{// 全パーツ分繰り返す

		int nCntModel = nCntParts;
		if (nStartIdx != 0)
		{
			nCntModel = nCntParts - nStartIdx;
		}

		if (nCntModel >= m_nNumModel)
		{
			break;
		}

		if (m_ppModel[nCntModel] == NULL)
		{// NULLだったら
			continue;
		}

		// 向き設定
		m_ppModel[nCntModel]->SetRotation(m_aInfo[nType].aKey[0].aParts[nCntParts].rot);
		aPartsOld[nCntParts].rot = m_aInfo[nType].aKey[0].aParts[nCntParts].rot;
		aPartsOld[nCntParts].scale = m_aInfo[nType].aKey[0].aParts[nCntParts].scale;

		// 元の位置取得
		MyLib::Vector3 posOrigin = m_pObjChara->GetOriginPosition();

		// 位置設定
		if (nCntParts == 0)
		{
			m_ppModel[nCntModel]->SetPosition(m_aInfo[nType].aKey[0].aParts[nCntParts].pos + posOrigin);
		}
		else
		{
			m_ppModel[nCntModel]->SetPosition(m_aInfo[nType].aKey[0].aParts[nCntParts].pos + m_ppModel[nCntModel]->GetOriginPosition());
		}
	}

	aPartsOld[0].pos = m_aInfo[nType].aKey[0].aParts[0].pos;
}

//==========================================================================
// 更新処理
//==========================================================================
void CMotion::Update(float fBuff)
{

	// 攻撃情報の総数取得
	int nNumAttackInfo = m_aInfo[m_nType].nNumAttackInfo;

	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (m_aInfo[m_nType].AttackInfo[nCntAttack] == NULL)
		{// NULLだったら
			continue;
		}

		if (m_aInfo[m_nType].AttackInfo[nCntAttack]->nInpactCnt < 0)
		{
			continue;
		}

		// まだ衝撃カウントの行動をしてない状態にする
		m_aInfo[m_nType].AttackInfo[nCntAttack]->bInpactAct = false;

		if (m_fCntAllFrame >= m_aInfo[m_nType].AttackInfo[nCntAttack]->nInpactCnt &&
			m_aInfo[m_nType].AttackInfo[nCntAttack]->bInpactActSet == false)
		{// 衝撃のカウントを超えた時 && まだ行動してなかったら
			
			// まだ衝撃カウントの行動をしてない状態にする
			m_aInfo[m_nType].AttackInfo[nCntAttack]->bInpactAct = true;
			m_aInfo[m_nType].AttackInfo[nCntAttack]->bInpactActSet = true;
		}
	}


	if (m_bFinish == true && m_aInfo[m_nType].nLoop == LOOP_OFF)
	{// 終了してた && ループOFFだったら
		return;
	}

	// 再生フレーム
	int nFrame;

	// 再生フレーム保存
	nFrame = m_aInfo[m_nType].aKey[m_nPatternKey].nFrame;

	if (m_nType == 0 && m_nOldType != 0 && m_nPatternKey == 0)
	{// ニュートラルで0個の時
		nFrame = 10;
	}

	// 次のキー
	int nNextKey = (m_nPatternKey + 1) % m_aInfo[m_nType].nNumKey;

	if (nNextKey == 0 && m_aInfo[m_nType].nLoop == LOOP_OFF)
	{// ループしないとき

		// 最後で固定
		nNextKey = m_aInfo[m_nType].nNumKey - 1;
	}

	int nStartIdx = m_pObjChara->GetMotionStartIdx();
	for (int nCntParts = nStartIdx; nCntParts < m_nNumModel + nStartIdx + 1; nCntParts++)
	{// 全パーツ分繰り返す

		int nCntModel = nCntParts;
		if (nStartIdx != 0)
		{
			nCntModel = nCntParts - nStartIdx;
		}

		if (nCntModel >= m_nNumModel)
		{
			break;
		}

		if (m_ppModel[nCntModel] == NULL)
		{// NULLだったら
			continue;
		}

		// 次と今の向きの差分取得
		float rotDiffX = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].rot.x -
			aPartsOld[nCntParts].rot.x;

		float rotDiffY = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].rot.y -
			aPartsOld[nCntParts].rot.y;

		float rotDiffZ = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].rot.z -
			aPartsOld[nCntParts].rot.z;

		// 角度の正規化
		UtilFunc::Transformation::RotNormalize(rotDiffX);
		UtilFunc::Transformation::RotNormalize(rotDiffY);
		UtilFunc::Transformation::RotNormalize(rotDiffZ);

		// パーツの向きを設定
		MyLib::Vector3 rot = mylib_const::DEFAULT_VECTOR3;

		// パーツの向きを設定
		rot.x =
			aPartsOld[nCntParts].rot.x +
			rotDiffX *
			(
			m_fCntFrame /
			(float)nFrame
			);

		// パーツの向きを設定
		rot.y =
			aPartsOld[nCntParts].rot.y +
			rotDiffY *
			(
			m_fCntFrame /
			(float)nFrame
			);

		// パーツの向きを設定
		rot.z =
			aPartsOld[nCntParts].rot.z +
			rotDiffZ *
			(
			m_fCntFrame /
			(float)nFrame
			);

		// 角度の正規化
		UtilFunc::Transformation::RotNormalize(rot.x);
		UtilFunc::Transformation::RotNormalize(rot.y);
		UtilFunc::Transformation::RotNormalize(rot.z);

		// 向き設定
		m_ppModel[nCntModel]->SetRotation(rot);

		{
			// 次と今の向きの差分取得
			float scaleDiffX = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].scale.x -
				aPartsOld[nCntParts].scale.x;

			float scaleDiffY = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].scale.y -
				aPartsOld[nCntParts].scale.y;

			float scaleDiffZ = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].scale.z -
				aPartsOld[nCntParts].scale.z;

			// パーツの向きを設定
			MyLib::Vector3 scale = MyLib::Vector3(0.0f, 0.0f, 0.0f);

			// パーツの向きを設定
			scale.x =
				aPartsOld[nCntParts].scale.x +
				scaleDiffX *
				(
				m_fCntFrame /
				(float)nFrame
				);

			// パーツの向きを設定
			scale.y =
				aPartsOld[nCntParts].scale.y +
				scaleDiffY *
				(
				m_fCntFrame /
				(float)nFrame
				);

			// パーツの向きを設定
			scale.z =
				aPartsOld[nCntParts].scale.z +
				scaleDiffZ *
				(
				m_fCntFrame /
				(float)nFrame
				);

			// 向き設定
			m_ppModel[nCntModel]->SetScale(scale);
		}

		if (nCntParts == 0)
		{
			// 本体の位置取得
			MyLib::Vector3 pos = m_pObjChara->GetPosition();

			// 本体の向き取得
			MyLib::Vector3 rot = m_pObjChara->GetRotation();

			// 元の位置取得
			MyLib::Vector3 posOrigin = m_pObjChara->GetOriginPosition();

			// パーツの位置取得
			MyLib::Vector3 posParts = m_ppModel[nCntModel]->GetPosition();
			MyLib::Vector3 posPartsOld = m_ppModel[nCntModel]->GetPosition();

			// 目標の位置との差分を求める
			float posDiffX = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.x -
				aPartsOld[nCntParts].pos.x;

			float posDiffY = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.y -
				aPartsOld[nCntParts].pos.y;

			float posDiffZ = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.z -
				aPartsOld[nCntParts].pos.z;

			// 親のYを補正
			posParts.y =
				aPartsOld[nCntParts].pos.y +
				posDiffY *
				(
				m_fCntFrame /
				(float)nFrame);

			// 位置設定
			m_ppModel[nCntModel]->SetPosition(posParts + posOrigin);

			int nNextMoveKey = m_nPatternKey + 1;
			if (nNextMoveKey >= m_aInfo[m_nType].nNumKey)
			{
				nNextMoveKey = m_nPatternKey;
			}

			// 動いた長さを求める
			float fMoveDiff =
				sqrtf((m_aInfo[m_nType].aKey[nNextMoveKey].aParts[nCntParts].pos.x - aPartsOld[nCntParts].pos.x) * (m_aInfo[m_nType].aKey[nNextMoveKey].aParts[nCntParts].pos.x - aPartsOld[nCntParts].pos.x)
					+ (m_aInfo[m_nType].aKey[nNextMoveKey].aParts[nCntParts].pos.z - aPartsOld[nCntParts].pos.z) * (m_aInfo[m_nType].aKey[nNextMoveKey].aParts[nCntParts].pos.z - aPartsOld[nCntParts].pos.z));
			fMoveDiff /= (static_cast<float>(nFrame) / static_cast<float>(fBuff));

			// 動きの向きを一時代入
			float fRot = m_aInfo[m_nType].aKey[nNextMoveKey].fRotMove;

			// 動きの向き方向へ移動
			pos.x += sinf(D3DX_PI + fRot + rot.y) * fMoveDiff;
			pos.z += cosf(D3DX_PI + fRot + rot.y) * fMoveDiff;

			m_pObjChara->SetPosition(pos);
		}
		else
		{
			// パーツの位置取得
			MyLib::Vector3 posParts = m_ppModel[nCntModel]->GetPosition();

			// 元の位置取得
			MyLib::Vector3 posOrigin = m_pObjChara->GetOriginPosition();

			// 目標の位置との差分を求める
			float posDiffX = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.x -
				aPartsOld[nCntParts].pos.x;

			float posDiffY = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.y -
				aPartsOld[nCntParts].pos.y;

			float posDiffZ = m_aInfo[m_nType].aKey[nNextKey].aParts[nCntParts].pos.z -
				aPartsOld[nCntParts].pos.z;

			// パーツの位置を設定
			posParts.y =
				aPartsOld[nCntParts].pos.y +
				posDiffY *
				(m_fCntFrame /
				(float)nFrame);

			posParts.x =
				aPartsOld[nCntParts].pos.x +
				posDiffX *
				(m_fCntFrame /
				(float)nFrame);

			posParts.z =
				aPartsOld[nCntParts].pos.z +
				posDiffZ *
				(m_fCntFrame /
				(float)nFrame);

			// 位置設定
			m_ppModel[nCntModel]->SetPosition(posParts + m_ppModel[nCntModel]->GetOriginPosition());
		}
	}

	// フレームのカウントを加算
	m_fCntFrame += 1.0f * fBuff;
	m_fCntAllFrame += 1.0f * fBuff;

	if (m_aInfo[m_nType].nLoop == LOOP_ON)
	{// ループモーションはいつでも終わってる

		// 終了判定ON
		m_bFinish = true;
	}

	if (m_fCntFrame >= (float)nFrame)
	{// フレームのカウントがフレーム数に達したら

		// フレームのカウントをゼロに戻す
		m_fCntFrame = 0;

		m_nOldType = m_nType;	// 前回のモーションの種類

		// パターンNO.更新
		m_nPatternKey = (m_nPatternKey + 1) % m_aInfo[m_nType].nNumKey;

		int nStartIdx = m_pObjChara->GetMotionStartIdx();
		for (int nCntParts = nStartIdx; nCntParts < m_nNumModel + nStartIdx + 1; nCntParts++)
		{// 全パーツ分繰り返す
			aPartsOld[nCntParts].rot = m_aInfo[m_nType].aKey[m_nPatternKey].aParts[nCntParts].rot;
			aPartsOld[nCntParts].pos = m_aInfo[m_nType].aKey[m_nPatternKey].aParts[nCntParts].pos;
			aPartsOld[nCntParts].scale = m_aInfo[m_nType].aKey[m_nPatternKey].aParts[nCntParts].scale;
		}

		if (m_nPatternKey == 0)
		{// キーがゼロの時

			// 総フレーム数リセット
			m_fCntAllFrame = 0.0f;

			//モーションの設定
			if (m_aInfo[m_nType].nLoop == LOOP_OFF)
			{// ループモーションじゃない時

				// 終了判定ON
				m_bFinish = true;
			}
			else
			{
				for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
				{
					if (m_aInfo[m_nType].AttackInfo[nCntAttack] == NULL)
					{// NULLだったら
						continue;
					}

					if (m_aInfo[m_nType].AttackInfo[nCntAttack]->nInpactCnt < 0)
					{
						continue;
					}

					// まだ衝撃カウントの行動をしてない状態にする
					m_aInfo[m_nType].AttackInfo[nCntAttack]->bInpactAct = false;
					m_aInfo[m_nType].AttackInfo[nCntAttack]->bInpactActSet = false;
				}
			}
		}
	}
}

//==========================================================================
// モーションの設定処理
//==========================================================================
void CMotion::Set(int nType, bool bBlend)
{
	// デバッグ用処理
	if (nType >= m_nNumAll)
	{// 総数を超えていたら
		return;
	}

	if (m_nType == nType)
	{// 現在の種類と同じだったら
		return;
	}

	m_nOldType = m_nType;	// 前回のモーションの種類
	m_nType = nType;		// 種類設定
	m_nPatternKey = 0;		// 何個目のキーか
	m_fCntFrame = 0.0f;		// フレームのカウント
	m_fCntAllFrame = 0.0f;	// 全てのカウント
	m_fMaxAllFrame = 0.0f;	// 全てのカウントの最大値
	m_bFinish = false;		// 終了したかどうか

	for (int nCntKey = 0; nCntKey < m_aInfo[m_nPatternKey].nNumKey; nCntKey++)
	{
		m_fMaxAllFrame += m_aInfo[m_nPatternKey].aKey[nCntKey].nFrame;	// 全てのカウントの最大値
	}

	// 攻撃情報の総数取得
	int nNumAttackInfo = m_aInfo[m_nType].nNumAttackInfo;
	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (m_aInfo[m_nType].AttackInfo[nCntAttack] == NULL)
		{// NULLだったら
			continue;
		}

		// まだ衝撃カウントの行動をしてない状態にする
		m_aInfo[m_nType].AttackInfo[nCntAttack]->bInpactAct = false;
		m_aInfo[m_nType].AttackInfo[nCntAttack]->bInpactActSet = false;
	}

	int nStartIdx = m_pObjChara->GetMotionStartIdx();
	for (int nCntParts = nStartIdx; nCntParts < m_nNumModel + nStartIdx + 1; nCntParts++)
	{// 全パーツ分繰り返す

		int nCntModel = nCntParts;
		if (nStartIdx != 0)
		{
			nCntModel = nCntParts - nStartIdx;
		}

		if (nCntModel >= m_nNumModel)
		{
			break;
		}

		if (m_ppModel[nCntModel] == NULL)
		{// NULLだったら
			continue;
		}

		// 過去の位置・向きを保存
		if (bBlend == true)
		{
			aPartsOld[nCntParts].rot = m_ppModel[nCntModel]->GetRotation();
			aPartsOld[nCntParts].scale = m_ppModel[nCntModel]->GetScale();

			if (nStartIdx == nCntParts)
			{
				aPartsOld[nCntParts].pos = m_ppModel[nCntModel]->GetPosition() - m_pObjChara->GetOriginPosition();
			}
			else
			{
				aPartsOld[nCntParts].pos = m_ppModel[nCntModel]->GetPosition() - m_ppModel[nCntModel]->GetOriginPosition();
			}
		}
		else
		{
			aPartsOld[nCntParts].rot = m_aInfo[m_nType].aKey[0].aParts[nCntParts].rot;
			aPartsOld[nCntParts].pos = m_aInfo[m_nType].aKey[0].aParts[nCntParts].pos + m_pObjChara->GetOriginPosition();
			aPartsOld[nCntParts].scale = m_aInfo[m_nType].aKey[0].aParts[nCntParts].scale;
		}
	}
}

//==========================================================================
// 現在のモーションタイプ取得
//==========================================================================
int CMotion::GetType(void)
{
	return m_nType;
}

//==========================================================================
// 前回のモーションタイプ取得
//==========================================================================
int CMotion::GetOldType(void)
{
	return m_nOldType;
}

//==========================================================================
// 終了しているかの判定
//==========================================================================
bool CMotion::IsFinish(void)
{
	return m_bFinish;
}

//==========================================================================
// 終了しているかの判定切り替え
//==========================================================================
void CMotion::ToggleFinish(bool bFinish)
{
	m_bFinish = bFinish;
}

//==========================================================================
// 遅延係数の設定
//==========================================================================
void CMotion::SetSlowFactor(float fFactor)
{
	m_fSlowFactor = fFactor;
}

//==========================================================================
// 移動の判定取得
//==========================================================================
int CMotion::IsGetMove(int nType)
{
	return m_aInfo[nType].nMove;
}

//==========================================================================
// モーション情報の設定
//==========================================================================
void CMotion::SetInfoData(int nType, Info info)
{
	m_aInfo[nType] = info;
}

//==========================================================================
// モーション情報の取得
//==========================================================================
CMotion::Info CMotion::GetInfo(int nType)
{
	return m_aInfo[nType];
}

//==========================================================================
// モーション情報の取得
//==========================================================================
CMotion::Info *CMotion::GetInfoPtr(int nType)
{
	return &m_aInfo[nType];
}

//==========================================================================
// モーション情報の登録
//==========================================================================
void CMotion::SetInfoSave(int nType, int nKey, int nParts, MyLib::Vector3 rot)
{
	m_aInfo[nType].aKey[nKey].aParts[nParts].rot = rot;
}

//==========================================================================
// キー総数変更
//==========================================================================
void CMotion::ChangeKeyNum(int nType, int nNum)
{
	m_aInfo[nType].nNumKey = nNum;
}

//==========================================================================
// キーフレーム変更
//==========================================================================
void CMotion::ChangeKeyFrame(int nType, int nKey, int nNum)
{
	m_aInfo[nType].aKey[nKey].nFrame = nNum;
}

//==========================================================================
// 攻撃情報変更
//==========================================================================
void CMotion::ChangeAttackInfo(int nType, int nIdx, AttackInfo info)
{
	*m_aInfo[nType].AttackInfo[nIdx] = info;
}

//==========================================================================
// 攻撃情報の総数加算
//==========================================================================
void CMotion::AddNumAttackInfo(int nType)
{
	if (m_aInfo[nType].nNumAttackInfo >= MAX_PARTS)
	{
		return;
	}

	// メモリ確保
	m_aInfo[nType].AttackInfo[m_aInfo[nType].nNumAttackInfo] = DEBUG_NEW AttackInfo;

	if (m_aInfo[nType].AttackInfo[m_aInfo[nType].nNumAttackInfo] != NULL)
	{// NULLじゃなければ

		// 初期化
		memset(m_aInfo[nType].AttackInfo[m_aInfo[nType].nNumAttackInfo], 0, sizeof(AttackInfo));

		// 攻撃情報の総数加算
		m_aInfo[nType].nNumAttackInfo++;
	}
}

//==========================================================================
// 攻撃情報の総数減算
//==========================================================================
void CMotion::SubNumAttackInfo(int nType)
{
	if (m_aInfo[nType].nNumAttackInfo <= 1)
	{
		return;
	}
	int nIdx = m_aInfo[nType].nNumAttackInfo - 1;

	// メモリ解放
	//if (m_aInfo[nType].AttackInfo[nIdx] != NULL)
	{
		delete m_aInfo[nType].AttackInfo[nIdx];
		m_aInfo[nType].AttackInfo[nIdx] = NULL;

		// 攻撃情報の総数減算
		m_aInfo[nType].nNumAttackInfo--;
	}
}

//==========================================================================
// 衝撃のフレームかどうか取得
//==========================================================================
bool CMotion::IsImpactFrame(AttackInfo attackInfo)
{
	return attackInfo.bInpactAct;
}

//==========================================================================
// 攻撃の位置取得
//==========================================================================
MyLib::Vector3 CMotion::GetAttackPosition(CModel **ppModel, AttackInfo attackInfo)
{
	D3DXMATRIX mtxTrans;	// 計算用マトリックス宣言

	if (ppModel[attackInfo.nCollisionNum] == NULL)
	{// NULLだったら
		return mylib_const::DEFAULT_VECTOR3;
	}

	// 判定するパーツのマトリックス取得
	D3DXMATRIX mtxWepon = ppModel[attackInfo.nCollisionNum]->GetWorldMtx();

	// 位置を反映する
	D3DXMatrixTranslation(&mtxTrans, attackInfo.Offset.x, attackInfo.Offset.y, attackInfo.Offset.z);
	D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

	return MyLib::Vector3(mtxWepon._41, mtxWepon._42, mtxWepon._43);
}

//==========================================================================
// 攻撃の位置取得
//==========================================================================
MyLib::Vector3 CMotion::GetAttackPosition(CModel *pModel, AttackInfo attackInfo)
{
	D3DXMATRIX mtxTrans;	// 計算用マトリックス宣言

	if (pModel == NULL)
	{// NULLだったら
		return mylib_const::DEFAULT_VECTOR3;
	}

	// 判定するパーツのマトリックス取得
	D3DXMATRIX mtxWepon = pModel->GetWorldMtx();

	// 位置を反映する
	D3DXMatrixTranslation(&mtxTrans, attackInfo.Offset.x, attackInfo.Offset.y, attackInfo.Offset.z);
	D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

	return MyLib::Vector3(mtxWepon._41, mtxWepon._42, mtxWepon._43);
}

//==========================================================================
// フレームカウント設定
//==========================================================================
void CMotion::SetFrameCount(float fCnt)
{
	m_fCntFrame = fCnt;
}

//==========================================================================
// カウント取得
//==========================================================================
float CMotion::GetAllCount(void)
{
	return m_fCntAllFrame;
}

//==========================================================================
// 全てのカウント取得
//==========================================================================
int CMotion::GetMaxAllCount(int nType)
{
	int nAllFrame = 0;
	for (int nCntKey = 0; nCntKey < m_aInfo[nType].nNumKey; nCntKey++)
	{
		nAllFrame += m_aInfo[nType].aKey[nCntKey].nFrame;	// 全てのカウントの最大値
	}
	return nAllFrame;
}

//==========================================================================
// 外部ファイル読み込み
//==========================================================================
void CMotion::ReadText(const std::string pTextFile)
{
	// 読み込み確認
	for (int nCntData = 0; nCntData < m_nNumLoad; nCntData++)
	{
		if (m_sTextFile[nCntData] != pTextFile)
		{// ファイル名が一致してない
			continue;
		}

		for (int nCntInfo = 0; nCntInfo < m_nNumLoadData[nCntData]; nCntInfo++)
		{
			for (int nCntATK = 0; nCntATK < m_aLoadData[nCntData][nCntInfo].nNumAttackInfo; nCntATK++)
			{// 攻撃情報分繰り返す

				// 攻撃情報登録
				SetAttackInfo(m_aLoadAttackData[nCntData][nCntInfo][nCntATK]);
			}

			// 情報登録
			SetInfo(m_aLoadData[nCntData][nCntInfo]);
		}
		return;
	}

	// ファイル名保存
	m_sTextFile.push_back(pTextFile);


	// ファイルを開く
	FILE* pFile = fopen(pTextFile.c_str(), "r");
	if (pFile == NULL)
	{//ファイルが開けなかった場合
		return;
	}

	// 読み込みのデータ数加算
	m_nNumLoadData.push_back(0);


	// 読み込み用変数
	char aComment[MAX_COMMENT];	// コメント用
	int nCntMotion = 0;
	int nCntFile = 0;	// ファイルの読み込み回数
	std::vector<std::string> motionFile;	// モーションファイル

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// モーション数の設定
		if (strcmp(aComment, "NUM_MOTION") == 0)
		{// NUM_MOTIONがきたら

			fscanf(pFile, "%s", &aComment[0]);	// =の分
			fscanf(pFile, "%d", &m_nNumMotion);	// モーション数読み込み

			while (nCntFile != m_nNumMotion)
			{// モーション数分読み込むまで繰り返し

				// 文字列の読み込み
				fscanf(pFile, "%s", &aComment[0]);

				// モーションファイルの読み込み
				if (strcmp(aComment, "MOTION_FILENAME") == 0)
				{// MOTION_FILENAMEがきたら

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%s", &aComment[0]);	// ファイル名

					// ファイル名保存
					motionFile.push_back(&aComment[0]);

					nCntFile++;	// ファイルの読み込み回数加算
				}
			}

			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);

	// 要素分繰り返し
	for (int i = 0; i < static_cast<int>(motionFile.size()); i++)
	{
		// モーション読み込み
		LoadMotion(motionFile[i], i);
	}

	// 読み込んだ数加算
	m_nNumLoad++;
}

//==========================================================================
// モーション読み込み
//==========================================================================
void CMotion::LoadMotion(const std::string text, int nIdxMotion)
{

	// ファイルを開く
	FILE* pFile = fopen(text.c_str(), "r");
	if (pFile == NULL)
	{//ファイルが開けなかった場合
		return;
	}

	// 読み込み用変数
	char aComment[MAX_COMMENT];
	int nCntKey = 0, nCntParts = 0;
	Info InitInfo = {};
	AttackInfo InitAttackInfo = {};

	// スケール値リセット
	for (int key = 0; key < MAX_KEY; key++)
	{
		for (int parts = 0; parts < MAX_PARTS; parts++)
		{
			InitInfo.aKey[key].aParts[parts].scale = mylib_const::DEFAULT_SCALE;
		}
	}

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		//==================================================
		// 各モーションの設定
		//==================================================
		if (strcmp(aComment, "MOTIONSET") == 0)
		{// モーション情報の読み込みを開始

			// 初期化
			m_aLoadData[m_nNumLoad].push_back(InitInfo);

			while (strcmp(aComment, "END_MOTIONSET") != 0)
			{// END_MOTIONSETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	// 確認する

				if (strcmp(aComment, "LOOP") == 0)
				{// LOOPが来たらループON/OFF読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad][nIdxMotion].nLoop);	// ループ0か1
				}

				if (strcmp(aComment, "NUM_KEY") == 0)
				{// NUM_KEYが来たらキー数読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad][nIdxMotion].nNumKey);	// キー数
				}

				if (strcmp(aComment, "MOVE") == 0)
				{// MOVEが来たら移動ON/OFF読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad][nIdxMotion].nMove);	// 移動0か1か
				}

				if (strcmp(aComment, "ATTACKINFO") == 0)
				{// ATTACKINFOが来たら

					// 初期化
					m_aLoadAttackData[m_nNumLoad][nIdxMotion].push_back(InitAttackInfo);

					int nNowAttackNum = m_aLoadData[m_nNumLoad][nIdxMotion].nNumAttackInfo;

					while (strcmp(aComment, "END_ATTACKINFO") != 0)
					{// END_ATTACKINFOが来るまで繰り返し

						fscanf(pFile, "%s", &aComment[0]);	// 確認する

						if (strcmp(aComment, "COLLISIONPARTS") == 0)
						{// COLLISIONPARTSが来たらパーツ番号読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum].nCollisionNum);	// 判定を取るパーツ番号
						}

						if (strcmp(aComment, "ATTACKRANGE") == 0)
						{// ATTACKRANGEが来たら攻撃サイズ読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%f", &m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum].fRangeSize);	// 判定のサイズ
						}

						if (strcmp(aComment, "OFFSET") == 0)
						{// OFFSETが来たらオフセット読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%f", &m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum].Offset.x);	// X座標
							fscanf(pFile, "%f", &m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum].Offset.y);	// Y座標
							fscanf(pFile, "%f", &m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum].Offset.z);	// Z座標
						}

						if (strcmp(aComment, "ATTACKCOUNT") == 0)
						{// ATTACKCOUNTが来たら攻撃カウント読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum].nMinCnt);	// 判定の最小カウント
							fscanf(pFile, "%d", &m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum].nMaxCnt);	// 判定の最大カウント
						}

						if (strcmp(aComment, "INPACTCOUNT") == 0)
						{// INPACTCOUNTが来たら衝撃カウント読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum].nInpactCnt);	// 衝撃のカウント
						}

						if (strcmp(aComment, "DAMAGE") == 0)
						{// DAMAGEが来たら攻撃力読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum].nDamage);	// 攻撃力
						}

						if (strcmp(aComment, "ONLYONETIME") == 0)
						{// ONLYONETIMEが来たら一回のみか読み込み
							int nFrag = 0;
							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &nFrag);	// 1度だけかの判定

							if (nFrag == 0) { m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum].bOnlyOneTime = false; }
							else { m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum].bOnlyOneTime = true; }
						}
					}

					// 攻撃情報の登録
					SetAttackInfo(m_aLoadAttackData[m_nNumLoad][nIdxMotion][nNowAttackNum]);

					// 攻撃情報の数加算
					m_aLoadData[m_nNumLoad][nIdxMotion].nNumAttackInfo++;
				}

				//==================================================
				//各キーの設定
				//==================================================
				if (strcmp(aComment, "KEYSET") == 0)
				{// KEYSETでキー情報の読み込み開始

					while (strcmp(aComment, "END_KEYSET") != 0)
					{// END_KEYSETが来るまで繰り返し

						fscanf(pFile, "%s", &aComment[0]);	// 確認する

						if (strcmp(aComment, "FRAME") == 0)
						{// FRAMEが来たら再生フレーム数読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad][nIdxMotion].aKey[nCntKey].nFrame);	// 再生フレーム
						}

						//==================================================
						// パーツの設定
						//==================================================
						if (strcmp(aComment, "PARTS") == 0)
						{// PARTSでパーツ情報の読み込み開始

							while (strcmp(aComment, "END_PARTS") != 0)
							{// END_PARTSが来るまで繰り返し

								fscanf(pFile, "%s", &aComment[0]);	// 確認する

								if (strcmp(aComment, "POS") == 0)
								{// POSが来たら位置読み込み

									fscanf(pFile, "%s", &aComment[0]);		// =の分
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nIdxMotion].aKey[nCntKey].aParts[nCntParts].pos.x);	// X座標
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nIdxMotion].aKey[nCntKey].aParts[nCntParts].pos.y);	// Y座標
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nIdxMotion].aKey[nCntKey].aParts[nCntParts].pos.z);	// Z座標
								}

								if (strcmp(aComment, "ROT") == 0)
								{// ROTが来たら角度読み込み

									fscanf(pFile, "%s", &aComment[0]);		// =の分
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nIdxMotion].aKey[nCntKey].aParts[nCntParts].rot.x);	// X角度
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nIdxMotion].aKey[nCntKey].aParts[nCntParts].rot.y);	// Y角度
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nIdxMotion].aKey[nCntKey].aParts[nCntParts].rot.z);	// Z角度
								}

								if (strcmp(aComment, "SCALE") == 0)
								{// SCALEが来たらスケール読み込み

									fscanf(pFile, "%s", &aComment[0]);		// =の分
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nIdxMotion].aKey[nCntKey].aParts[nCntParts].scale.x);	// Xスケール
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nIdxMotion].aKey[nCntKey].aParts[nCntParts].scale.y);	// Yスケール
									fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad][nIdxMotion].aKey[nCntKey].aParts[nCntParts].scale.z);	// Zスケール
								}
							}// END_PARTSのかっこ

							nCntParts++;	//パーツのカウント加算
						}
					}// END_KEYSETのかっこ

					nCntParts = 0;	// パーツのカウントリセット
					nCntKey++;	// キーのカウント加算
				}

			}// END_MOTIONSETのかっこ

			// モーション情報の登録
			SetInfo(m_aLoadData[m_nNumLoad][nIdxMotion]);

			if (m_nNumMotion == 0)
			{
				m_fMaxAllFrame = 0;
				for (int nCntKey = 0; nCntKey < m_aInfo[m_nPatternKey].nNumKey; nCntKey++)
				{
					m_fMaxAllFrame += m_aInfo[m_nPatternKey].aKey[nCntKey].nFrame;	// 全てのカウントの最大値
				}
			}

			nCntKey = 0;	// キーのカウントリセット
			nIdxMotion++;	// モーションのカウント加算

			// モーション毎のデータ数
			m_nNumLoadData[m_nNumLoad]++;
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける

			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);

}
