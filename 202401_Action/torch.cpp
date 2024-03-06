//=============================================================================
// 
//  松明処理 [torch.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "torch.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"
#include "calculation.h"
#include "game.h"
#include "particle.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* LOADTEXT = "data\\TEXT\\map\\torch.txt";
	const char* MODEL_STAND = "data\\MODEL\\arena\\taimatu_002.x";
	const char* MODEL_WALL = "data\\MODEL\\arena\\taimatu_001.x";
}
std::vector<std::string> CTorch::ModelFile = {};		// モデルファイル名
CListManager<CTorch> CTorch::m_List = {};		// リスト

//==========================================================================
// コンストラクタ
//==========================================================================
CTorch::CTorch(int nPriority) : CObjectX(nPriority)
{
	// 値のクリア
	m_nCntFire = 0;
}

//==========================================================================
// デストラクタ
//==========================================================================
CTorch::~CTorch()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CTorch *CTorch::Create(TYPE type, const MyLib::Vector3& pos, const MyLib::Vector3& rot)
{

	CTorch *pTorch = nullptr;

	// メモリ確保
	switch (type)
	{
	case CTorch::TYPE_STAND:
		pTorch = DEBUG_NEW CTorchStand;
		break;

	case CTorch::TYPE_WALL:
		pTorch = DEBUG_NEW CTorchWall;
		break;

	default:
		return nullptr;
		break;
	}

	if (pTorch != nullptr)
	{// メモリの確保が出来ていたら

		pTorch->SetPosition(pos);
		pTorch->SetRotation(rot);

		// 初期化処理
		pTorch->Init();
	}

	return pTorch;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTorch::Init()
{
	// 種類の設定
	CObject::SetType(TYPE_OBJECTX);

	// リストに割り当て
	m_List.Regist(this);

	return S_OK;
}

//==========================================================================
// 置き型初期化処理
//==========================================================================
HRESULT CTorchStand::Init()
{
	HRESULT hr;

	// 初期化処理
	if (FAILED(CTorch::Init()))
	{
		return E_FAIL;
	}

	hr = CObjectX::Init(MODEL_STAND);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	return S_OK;
}

//==========================================================================
// 置き型初期化処理
//==========================================================================
HRESULT CTorchWall::Init()
{
	HRESULT hr;

	// 初期化処理
	if (FAILED(CTorch::Init()))
	{
		return E_FAIL;
	}

	hr = CObjectX::Init(MODEL_WALL);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTorch::Uninit()
{
	// リストから削除
	m_List.Delete(this);

	// 終了処理
	CObjectX::Uninit();
}

//==========================================================================
// 削除
//==========================================================================
void CTorch::Kill()
{
	// リストから削除
	m_List.Delete(this);

	// 削除処理
	CObjectX::Kill();
}

//==========================================================================
// 置き型更新処理
//==========================================================================
void CTorchStand::Update()
{
	m_nCntFire = (m_nCntFire + 1) % 10;

	if (m_nCntFire == 0)
	{
		my_particle::Create(GetPosition(), my_particle::TYPE_FIRE_STANDTORCH);
	}
}

//==========================================================================
// 壁掛け更新処理
//==========================================================================
void CTorchWall::Update()
{
	m_nCntFire = (m_nCntFire + 1) % 10;

	if (m_nCntFire == 0)
	{
		my_particle::Create(GetPosition(), my_particle::TYPE_FIRE_WALLTORCH);
	}
}

//==========================================================================
// モデル読み込み処理
//==========================================================================
HRESULT CTorch::ReadXFile(const char* pTextFile)
{
	if (!ModelFile.empty())
	{
		return S_OK;
	}

	// リセット
	int nNumFileAll = 0;

	char aComment[MAX_COMMENT] = {};	// コメント用
	int nFileNum = 0;					// ファイルの数
	int nCntTexture = 0;				// テクスチャ読み込みカウント

	//ファイルを開く
	FILE* pFile = fopen(pTextFile, "r");

	if (pFile == nullptr)
	{//ファイルが開けた場合
		return E_FAIL;
	}

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// モデル数の設定
		if (strcmp(&aComment[0], "NUM_MODEL") == 0)
		{// NUM_MODELがきたら

			fscanf(pFile, "%s", &aComment[0]);	// =の分
			fscanf(pFile, "%d", &nFileNum);	// モデル数
		}

		while (nNumFileAll != nFileNum)
		{// モデルの数分読み込むまで繰り返し

			// 文字列の読み込み
			fscanf(pFile, "%s", &aComment[0]);

			// モデル名の設定
			if (strcmp(&aComment[0], "MODEL_FILENAME") == 0)
			{// NUM_MODELがきたら

				fscanf(pFile, "%s", &aComment[0]);	// =の分
				fscanf(pFile, "%s", &aComment[0]);	// ファイル名

				// 最後尾に追加
				ModelFile.push_back(&aComment[0]);

				// ファイル数
				nNumFileAll++;
			}
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける

			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);

	return S_OK;
}

//==========================================================================
// 外部テキスト読み込み処理
//==========================================================================
void CTorch::SetTorch()
{
	// モデル読み込み
	if (FAILED(ReadXFile(LOADTEXT)))
	{// 失敗した場合
		return;
	}

	char aComment[MAX_COMMENT] = {};	//コメント用

	// ファイルポインタ
	FILE* pFile = nullptr;

	//ファイルを開く
	pFile = fopen(LOADTEXT, "r");

	if (pFile == nullptr)
	{
		return;
	}

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// モデルの設定
		if (strcmp(&aComment[0], "MODELSET") == 0)
		{// モデルの読み込みを開始

			int nType = 0;
			MyLib::Vector3 pos = mylib_const::DEFAULT_VECTOR3;
			MyLib::Vector3 rot = mylib_const::DEFAULT_VECTOR3;

			while (strcmp(&aComment[0], "END_MODELSET"))
			{// END_MODELSETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	// 確認する

				if (strcmp(&aComment[0], "TYPE") == 0)
				{// TYPEが来たら種類読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%d", &nType);		// モデル種類の列挙
				}

				if (strcmp(&aComment[0], "POS") == 0)
				{// POSが来たら位置読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%f", &pos.x);		// X座標
					fscanf(pFile, "%f", &pos.y);		// Y座標
					fscanf(pFile, "%f", &pos.z);		// Z座標
				}

				if (strcmp(&aComment[0], "ROT") == 0)
				{// ROTが来たら向き読み込み

					fscanf(pFile, "%s", &aComment[0]);	// =の分
					fscanf(pFile, "%f", &rot.x);		// Xの向き
					fscanf(pFile, "%f", &rot.y);		// Yの向き
					fscanf(pFile, "%f", &rot.z);		// Zの向き
				}
			}// END_MODELSETのかっこ
			
			// タイプの物を生成
			CTorch::Create(static_cast<TYPE>(nType), pos, rot);
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける

			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);
}
