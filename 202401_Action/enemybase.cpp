//=============================================================================
// 
//  カメラの軸処理 [enemybase.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "enemybase.h"
#include "calculation.h"
#include "game.h"
#include "gamemanager.h"
#include "tutorial.h"
#include "3D_effect.h"
#include "objectX.h"
#include "enemymanager.h"
#include "manager.h"

//==========================================
//  定数定義
//==========================================
namespace
{
	const char* MARKOBJ = "data\\MODEL\\koko.x";
}

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyBase::CEnemyBase()
{
	// 値のクリア
	memset(&m_apObjX[0], NULL, sizeof(m_apObjX));	// オブジェクトX
	m_nNumAll = 0;		// 総数
	m_nNumStage = 0;	// ステージの総数
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyBase::~CEnemyBase()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEnemyBase *CEnemyBase::Create(const char *pFileName)
{
	// 生成用のオブジェクト
	CEnemyBase *pModel = NULL;

	if (pModel == NULL)
	{// NULLだったら

		// メモリの確保
		pModel = DEBUG_NEW CEnemyBase;

		if (pModel != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pModel->ReadText(pFileName);

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 初期化処理
			hr = pModel->Init();
			if (FAILED(hr))
			{// 失敗していたら
				delete pModel;
				return NULL;
			}
		}

		return pModel;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyBase::Init(void)
{
	// 敵マネージャ取得
	CEnemyManager *pEnemyManager = CGame::GetEnemyManager();

	//// 生成する
	//for (int i = 0; i < m_nNumAll; i++)
	//{
	//	// デバッグ用数字の生成
	//	m_pMultiNumber[i] = CDebugPointNumber::Create(i);

	//	if (m_EnemyBaseInfo[i].nRush == 0)
	//	{// ラッシュ用じゃなかったら
	//		pEnemyManager->SetEnemy(
	//			m_EnemyBaseInfo[i].pos,
	//			mylib_const::DEFAULT_VECTOR3,
	//			m_EnemyBaseInfo[i].nPattern);
	//	}
	//}

	return S_OK;
}

//==========================================================================
// 位置作成
//==========================================================================
void CEnemyBase::CreatePos(int nStage, int nPattern, MyLib::Vector3 pos, int nRush)
{

	if (nStage < 0)
	{
		nStage = 0;
	}

	if (nStage >= (int)m_EnemyBaseInfo.size())
	{
		nStage = (int)m_EnemyBaseInfo.size();
		m_EnemyBaseInfo.push_back(std::vector<sInfo>());

		// 拠点数追加
		m_nBaseNum.push_back(0);	// 拠点の数
		m_nNumStage++;
	}

	// 位置生成
	m_EnemyBaseInfo[nStage].push_back(sInfo());
	m_EnemyBaseInfo[nStage][m_nBaseNum[nStage]].nPattern = nPattern;	// 種類
	m_EnemyBaseInfo[nStage][m_nBaseNum[nStage]].pos = pos;
	m_EnemyBaseInfo[nStage][m_nBaseNum[nStage]].nRush = nRush;

	// 目印生成
	m_apObjX[m_nNumAll] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
	m_apObjX[m_nNumAll]->SetType(CObject::TYPE_BALLAST);
	m_apObjX[m_nNumAll]->SetPosition(m_EnemyBaseInfo[nStage][m_nBaseNum[nStage]].pos);

	// 総数加算
	m_nNumAll++;
	m_nBaseNum[nStage]++;

}

//==========================================================================
// 位置削除
//==========================================================================
void CEnemyBase::DeletePos(int nStage, int nIdx)
{
	// ソート処理
	UtilFunc::Calculation::CalSort(&m_EnemyBaseInfo[nStage][0], nIdx, m_nNumAll);

	// 位置生成
	m_EnemyBaseInfo[nStage].pop_back();

	// 総数加算
	m_nNumAll--;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyBase::Uninit(void)
{
	
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyBase::Update(void)
{
#if _DEBUG

	int nCntObj = 0;
	for (int nCntStage = 0; nCntStage < m_nNumStage; nCntStage++)
	{
		for (int i = 0; i < m_nBaseNum[nCntStage]; i++)
		{
			if (m_apObjX[nCntObj] == NULL)
			{
				m_apObjX[nCntObj] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// オブジェクトX
				m_apObjX[nCntObj]->SetType(CObject::TYPE_BALLAST);
				//m_apObjX[i]->SetPositionD3DXVECTOR3(pos.x, m_apObjX[i]->GetPosition().y, pos.z);
			}

			m_apObjX[nCntObj]->SetPosition(m_EnemyBaseInfo[nCntStage][i].pos);

			nCntObj++;
		}
	}
#endif

	// ステージの総数設定
	CGame::GetGameManager()->SetNumStage(m_nNumStage);

}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
HRESULT CEnemyBase::ReadText(const char *pFileName)
{
	char aComment[MAX_COMMENT] = {};	// コメント用

	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen(pFileName, "r");

	if (pFile == NULL)
	{// ファイルが開けなかった場合
		return E_FAIL;
	}

	// リセット
	m_nNumAll = 0;
	m_nNumStage = 0;	// ステージの総数

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);


		if (strcmp(aComment, "STAGESET") == 0)
		{// STAGESETでステージ毎の読み込み開始

			// 最後尾に生成
			m_EnemyBaseInfo.push_back(std::vector<sInfo>());

			// 拠点数追加
			m_nBaseNum.push_back(0);	// 拠点の数

			while (strcmp(aComment, "END_STAGESET") != 0)
			{// END_STAGESETが来るまで繰り返す

				// 文字列の読み込み
				fscanf(pFile, "%s", &aComment[0]);

				if (strcmp(aComment, "BASESET") == 0)
				{// BASESETで敵拠点の読み込み開始

					// 最後尾に生成
					m_EnemyBaseInfo[m_nNumStage].push_back(sInfo());

					int nCntBase = m_nBaseNum[m_nNumStage];

					while (strcmp(aComment, "END_BASESET") != 0)
					{// END_BASESETが来るまで繰り返す

						fscanf(pFile, "%s", &aComment[0]);	// 確認する

						if (strcmp(aComment, "PATTERN") == 0)
						{// PATTERNが来たら敵の種類読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_EnemyBaseInfo[m_nNumStage][nCntBase].nPattern);	// キャラファイル番号
						}

						if (strcmp(aComment, "POS") == 0)
						{// POSが来たら位置読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%f", &m_EnemyBaseInfo[m_nNumStage][nCntBase].pos.x);	// X座標
							fscanf(pFile, "%f", &m_EnemyBaseInfo[m_nNumStage][nCntBase].pos.y);	// Y座標
							fscanf(pFile, "%f", &m_EnemyBaseInfo[m_nNumStage][nCntBase].pos.z);	// Z座標
						}

						if (strcmp(aComment, "ROT") == 0)
						{// ROTが来たら位置読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%f", &m_EnemyBaseInfo[m_nNumStage][nCntBase].rot.x);	// X
							fscanf(pFile, "%f", &m_EnemyBaseInfo[m_nNumStage][nCntBase].rot.y);	// Y
							fscanf(pFile, "%f", &m_EnemyBaseInfo[m_nNumStage][nCntBase].rot.z);	// Z
						}

						if (strcmp(aComment, "RUSH") == 0)
						{// RUSHが来たらラッシュ用か読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_EnemyBaseInfo[m_nNumStage][nCntBase].nRush);	// ラッシュ用
						}

					}// END_BASESETのかっこ

					// 敵の拠点数加算
					m_nNumAll++;
					m_nBaseNum[m_nNumStage]++;
				}


			}// END_STAGESETのかっこ

			// ステージの総数加算
			m_nNumStage++;
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける

			break;
		}
	}

	if (CManager::GetInstance()->GetScene()->GetMode() != CScene::MODE_TUTORIAL)
	{
		// ステージの総数設定
		CGame::GetGameManager()->SetNumStage(m_nNumStage);
	}

	return S_OK;
}

//==========================================================================
// 外部ファイル書き出し処理
//==========================================================================
void CEnemyBase::Save(void)
{

	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen("data\\TEXT\\enemydata\\base.txt", "w");

	if (pFile == NULL)
	{// ファイルが開けなかった場合
		return;
	}

	fprintf(pFile,
		"\n"
		"#==============================================================================\n"
		"# 敵拠点の配置\n"
		"#==============================================================================");

	for (int nCntStage = 0; nCntStage < m_nNumStage; nCntStage++)
	{
		// 出力
		fprintf(pFile,
			"\n"
			"#--------------------------------[Stage%d]-----------------------------\n"
			"STAGESET\n", nCntStage);

		for (int i = 0; i < m_nBaseNum[nCntStage]; i++)
		{
			// 出力
			fprintf(pFile,
				"\tBASESET\n"
				"\t\tPATTERN = %d\n"
				"\t\tPOS = %.2f %.2f %.2f\n"
				"\t\tRUSH = %d\n"
				"\tEND_BASESET\n\n",
				m_EnemyBaseInfo[nCntStage][i].nPattern,
				m_EnemyBaseInfo[nCntStage][i].pos.x, m_EnemyBaseInfo[nCntStage][i].pos.y, m_EnemyBaseInfo[nCntStage][i].pos.z,
				m_EnemyBaseInfo[nCntStage][i].nRush);
		}

		// 出力
		fprintf(pFile,
			"END_STAGESET\n");
	}

	fprintf(pFile, "\nEND_SCRIPT		# この行は絶対消さないこと！");

	//ファイルを閉じる
	fclose(pFile);
}

//==========================================================================
// 軸数取得
//==========================================================================
int CEnemyBase::GetSpawnPointNum(void)
{
	return m_EnemyBaseInfo.size() - 1;
}

//==========================================================================
// 位置取得
//==========================================================================
MyLib::Vector3 CEnemyBase::GetSpawnPoint(int nStage, int nIdx)
{
	if (nIdx < 0)
	{
		nIdx = 0;
	}

	if (nIdx >= (int)m_EnemyBaseInfo[nStage].size() ||
		nStage >= (int)m_EnemyBaseInfo.size())
	{// 要素数を超えていたら

		return mylib_const::DEFAULT_VECTOR3;
	}

	return m_EnemyBaseInfo[nStage][nIdx].pos;
}

//==========================================================================
// 位置設定
//==========================================================================
void CEnemyBase::SetSpawnPoint(int nStage, int nIdx, MyLib::Vector3 pos)
{
	if (nIdx < 0)
	{
		return;
	}

	// 情報渡す
	m_EnemyBaseInfo[nStage][nIdx].pos = pos;
}

//==========================================================================
// 変更の情報取得
//==========================================================================
CEnemyBase::sInfo CEnemyBase::GetEnemyBaseInfo(int nStage, int nIdx)
{
	sInfo InitInfo;
	memset(&InitInfo, NULL, sizeof(InitInfo));

	if (m_EnemyBaseInfo.size() <= 0 || (int)m_EnemyBaseInfo[nStage].size() <= nIdx || (int)m_EnemyBaseInfo.size() <= nStage)
	{// サイズ無し
		return InitInfo;
	}

	return m_EnemyBaseInfo[nStage][nIdx];
}
