//=============================================================================
// 
//  敵のマネージャ処理 [enemymanager.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "enemymanager.h"
#include "calculation.h"
#include "manager.h"
#include "game.h"
#include "gamemanager.h"
#include "tutorial.h"
#include "renderer.h"
#include "enemy.h"
#include "enemy_boss.h"
#include "particle.h"
#include "fade.h"
#include "elevation.h"
#include "player.h"
#include "impactwave.h"
#include "camera.h"
#include "sound.h"
#include "enemybase.h"
#include "instantfade.h"
#include "timer.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyManager::CEnemyManager()
{
	// 値のクリア
	m_pBoss = NULL;										// ボス
	memset(&m_aPattern[0], NULL, sizeof(m_aPattern));	// 配置の種類
	m_state = STATE_NONE;	// 状態
	m_nPatternNum = 0;		// 出現パターン数
	m_nNumChara = 0;		// 敵の種類の総数
	m_bChangeStage = false;	// ステージ変更中か
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyManager::~CEnemyManager()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEnemyManager *CEnemyManager::Create(const std::string pTextFile)
{
	// 生成用のオブジェクト
	CEnemyManager *pModel = NULL;

	if (pModel == NULL)
	{// NULLだったら

		// メモリの確保
		pModel = DEBUG_NEW CEnemyManager;

		if (pModel != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pModel->ReadText(pTextFile);
			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 初期化処理
			hr = pModel->Init();

			if (FAILED(hr))
			{// 失敗していたら
				pModel->Uninit();
				pModel = nullptr;
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
HRESULT CEnemyManager::Init(void)
{

	// ステージ変更中にする
	m_bChangeStage = false;

	// 遷移状態に変更
	//CGame::GetGameManager()->SetType(CGameManager::SCENE_TRANSITION);

	// 敵拠点データ取得
	CEnemyBase *pEnemyBase = CGame::GetEnemyBase();

	if (pEnemyBase == NULL)
	{
		return E_FAIL;
	}

	// 拠点の数取得
	int nNumBase = pEnemyBase->GetNumBase(0);

	for (int i = 0; i < nNumBase; i++)
	{
		// 拠点ごとのデータ取得
		CEnemyBase::sInfo sEnemyBaseInfo = pEnemyBase->GetEnemyBaseInfo(0, i);

		// 敵の配置
		SetEnemy(sEnemyBaseInfo.pos, sEnemyBaseInfo.rot, sEnemyBaseInfo.nPattern);
	}


	// 変更中じゃなくする
	//SetEnableChangeStage(false);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyManager::Uninit(void)
{
	
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyManager::Update(void)
{
	int nNumAll = CEnemy::GetListObj().GetNumAll();
	if (nNumAll <= 0 && !m_bChangeStage)
	{// 全員倒されたら

		// ステージ変更中にする
		m_bChangeStage = true;

		// 通常クリア状態にする
		CGame::GetGameManager()->SetType(CGameManager::SCENE_MAINCLEAR);
		CGame::GetGameManager()->GameClearSettings();
	}

	// テキストの描画
	CManager::GetInstance()->GetDebugProc()->Print(
		"---------------- 敵情報 ----------------\n"
		"【残り人数】[%d], 【パターン数】[%d]\n", nNumAll, m_nPatternNum);
}

//==========================================================================
// ステージ毎の敵配置
//==========================================================================
void CEnemyManager::SetStageEnemy(void)
{
	// ゲームマネージャ取得
	CGameManager* pGameManager = CGame::GetGameManager();

	if (pGameManager == NULL)
	{
		return;
	}

	if (pGameManager->IsEndNormalStage() == true)
	{
		return;
	}

	// ステージの総数取得
	int nNumStage = pGameManager->GetNumStage();
	int nNowStage = pGameManager->GetNowStage();

	if (nNumStage <= nNowStage)
	{
		return;
	}

	// 敵拠点データ取得
	CEnemyBase* pEnemyBase = CGame::GetEnemyBase();
	if (pEnemyBase == NULL)
	{
		return;
	}

	// 拠点の数取得
	int nNumBase = pEnemyBase->GetNumBase(nNowStage);

	for (int i = 0; i < nNumBase; i++)
	{
		// 拠点ごとのデータ取得
		CEnemyBase::sInfo sEnemyBaseInfo = pEnemyBase->GetEnemyBaseInfo(nNowStage, i);

		// 敵の配置
		SetEnemy(sEnemyBaseInfo.pos, sEnemyBaseInfo.rot, sEnemyBaseInfo.nPattern);
	}

	// ステージ加算
	pGameManager->AddNowStage();
}

//==========================================================================
// ボスステージの敵配置
//==========================================================================
void CEnemyManager::SetStageBoss(void)
{
	// 敵拠点データ取得
	CEnemyBase *pEnemyBase = CGame::GetEnemyBase();
	if (pEnemyBase == NULL)
	{
		return;
	}

	// 拠点の数取得
	int nNumBase = pEnemyBase->GetNumBase(0);

	for (int i = 0; i < nNumBase; i++)
	{
		// 拠点ごとのデータ取得
		CEnemyBase::sInfo sEnemyBaseInfo = pEnemyBase->GetEnemyBaseInfo(0, i);

		// 敵の配置
		SetEnemy(sEnemyBaseInfo.pos, sEnemyBaseInfo.rot, sEnemyBaseInfo.nPattern);
	}
}

//==========================================================================
// 敵配置
//==========================================================================
CEnemy **CEnemyManager::SetEnemy(MyLib::Vector3 pos, MyLib::Vector3 rot, int nPattern)
{
	int nNumSpawn = m_aPattern[nPattern].nNumEnemy;	// スポーンする数
	int nCntStart = 0;
	Pattern NowPattern = m_aPattern[nPattern];
	CEnemy *pEnemy[mylib_const::MAX_PATTEN_ENEMY];
	memset(&pEnemy[0], NULL, sizeof(pEnemy));

	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{
		int nType = NowPattern.EnemyData[nCntEnemy].nType;

		// 計算用マトリックス
		D3DXMATRIX mtxRot, mtxTrans, mtxWorld;

		// マトリックスの初期化
		D3DXMatrixIdentity(&mtxWorld);

		// 敵拠点の向きを反映する
		D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

		// パターン内の位置を反映する
		D3DXMatrixTranslation(&mtxTrans, NowPattern.EnemyData[nCntEnemy].pos.x, NowPattern.EnemyData[nCntEnemy].pos.y, NowPattern.EnemyData[nCntEnemy].pos.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

		// スポーン時の向きを掛け合わせる
		MyLib::Vector3 spawnPos = MyLib::Vector3(mtxWorld._41, mtxWorld._42, mtxWorld._43);

		// 拠点の位置分加算
		spawnPos += pos;

		// 敵の生成
		pEnemy[nCntEnemy] = CEnemy::Create(
			sMotionFileName[nType].c_str(),	// ファイル名
			spawnPos,						// 位置
			(CEnemy::TYPE)nType);			// 種類

		if (pEnemy[nCntEnemy] == NULL)
		{// 失敗していたら

			delete pEnemy[nCntEnemy];
			pEnemy[nCntEnemy] = NULL;
			break;
		}

		// ボスの場合コピー
		if (nType == 0 && m_pBoss == NULL)
		{
			m_pBoss = (CEnemyBoss*)pEnemy[nCntEnemy];
		}

		// 向き設定
		pEnemy[nCntEnemy]->SetRotation(rot);
		pEnemy[nCntEnemy]->SetRotDest(rot.y);
	}

	return &pEnemy[0];
}

//==========================================================================
// パターン数
//==========================================================================
int CEnemyManager::GetPatternNum(void)
{
	return m_nPatternNum;
}

//==========================================================================
// パターン取得
//==========================================================================
CEnemyManager::Pattern CEnemyManager::GetPattern(int nPattern)
{
	return m_aPattern[nPattern];
}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
HRESULT CEnemyManager::ReadText(const std::string pTextFile)
{

	FILE *pFile = NULL;	// ファイルポインタを宣言

	// ファイルを開く
	pFile = fopen(pTextFile.c_str(), "r");
	if (pFile == NULL)
	{//ファイルが開けた場合
		return E_FAIL;
	}

	char aComment[MAX_COMMENT];	// コメント
	int nType = 0;				// 配置する種類
	int nCntPatten = 0;			// パターンのカウント
	int nCntFileName = 0;

	memset(&m_aPattern[0], NULL, sizeof(m_aPattern));	// 読み込みデータ
	m_nNumChara = 0;

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// キャラクター数の設定
		if (strcmp(aComment, "NUM_CHARACTER") == 0)
		{// NUM_CHARACTERがきたら

			fscanf(pFile, "%s", &aComment[0]);	// =の分
			fscanf(pFile, "%d", &m_nNumChara);	// キャラクター数
		}

		while (nCntFileName != m_nNumChara)
		{// モデルの数分読み込むまで繰り返し

			// 文字列の読み込み
			fscanf(pFile, "%s", &aComment[0]);

			// モデル名の設定
			if (strcmp(aComment, "MOTION_FILENAME") == 0)
			{// NUM_MODELがきたら

				fscanf(pFile, "%s", &aComment[0]);	// =の分
				fscanf(pFile, "%s", &aComment[0]);	// ファイル名

				// ファイル名保存
				sMotionFileName[nCntFileName] = aComment;

				nCntFileName++;	// ファイル数加算
			}
		}

		// 各パターンの設定
		if (strcmp(aComment, "PATTERNSET") == 0)
		{// 配置情報の読み込みを開始

			int nCntEnemy = 0;			// 敵の配置カウント

			while (strcmp(aComment, "END_PATTERNSET") != 0)
			{// END_PATTERNSETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	//確認する

				if (strcmp(aComment, "FIXEDMOVE") == 0)
				{// FIXEDMOVEが来たら一定の動きの種類読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &m_aPattern[nCntPatten].nFixedType);	// 一定の動きの種類
				}

				if (strcmp(aComment, "ENEMYSET") == 0)
				{// ENEMYSETで敵情報の読み込み開始

					while (strcmp(aComment, "END_ENEMYSET") != 0)
					{// END_ENEMYSETが来るまで繰り返す

						fscanf(pFile, "%s", &aComment[0]);	// 確認する

						if (strcmp(aComment, "TYPE") == 0)
						{// TYPEが来たらキャラファイル番号読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nType);	// キャラファイル番号
						}

						if (strcmp(aComment, "POS") == 0)
						{// POSが来たら位置読み込み

							fscanf(pFile, "%s", &aComment[0]);		// =の分
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.x);	// X座標
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.y);	// Y座標
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.z);	// Z座標
						}

						if (strcmp(aComment, "STARTFRAME") == 0)
						{// STARTFRAMEが来たら初期フレーム読み込み

							fscanf(pFile, "%s", &aComment[0]);	// =の分
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nStartFrame);	// 初期フレーム
						}

					}// END_ENEMYSETのかっこ

					nCntEnemy++;	// 敵のカウントを加算
					m_aPattern[nCntPatten].nNumEnemy++;	// 敵のカウントを加算
				}
			}// END_PATTERNSETのかっこ

			nCntPatten++;	// パターン加算
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける
			break;
		}
	}

	// パターン数
	m_nPatternNum = nCntPatten;

	// ファイルを閉じる
	fclose(pFile);

	return S_OK;
}

//==========================================================================
// ボス取得
//==========================================================================
CEnemyBoss *CEnemyManager::GetBoss(void)
{
	return m_pBoss;
}

//==========================================================================
// 敵のモーションファイル名取得
//==========================================================================
const char *CEnemyManager::GetMotionFilename(int nType)
{
	return &sMotionFileName[nType][0];
}
