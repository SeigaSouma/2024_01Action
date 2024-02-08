//=============================================================================
// 
//  観衆処理 [gallery.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "gallery.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "calculation.h"
#include "objectX.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* LOADTEXT = "data\\TEXT\\map\\gallery.txt";
	const char* FILENAME[3][3] =
	{
		{
			"data\\MODEL\\gallery\\gallery_red_wait.x",
			"data\\MODEL\\gallery\\gallery_red_move01.x",
			"data\\MODEL\\gallery\\gallery_red_move02.x",
		},
		{
			"data\\MODEL\\gallery\\gallery_blue_wait.x",
			"data\\MODEL\\gallery\\gallery_blue_move01.x",
			"data\\MODEL\\gallery\\gallery_blue_move02.x",
		},
		{
			"data\\MODEL\\gallery\\gallery_green_wait.x",
			"data\\MODEL\\gallery\\gallery_green_move01.x",
			"data\\MODEL\\gallery\\gallery_green_move02.x",
		},
	};
	const int NUM_GALLERY = 7;	// 観衆の人数
	const float DISTANCE = 80.0f;	// 間隔
	const float DISTANCE_DEPTH = 70.0f;	// 奥行きの間隔
	const float GRAVITY = 0.5f;		// 重力
	const float JUMP = 6.0f;		// ジャンプ力
	const float TIME_COUNTERHEAT = 1.8f;	// カウンター盛り上がりの時間
}
std::vector<int> CGallery::m_nModelIdx = {};		// モデルインデックス番号
CListManager<CGallery> CGallery::m_List = {};		// リスト

//==========================================================================
// 関数ポインタ
//==========================================================================
CGallery::STATE_FUNC CGallery::m_StateFunc[] =
{
	&CGallery::StateNone,	// なし
	&CGallery::StateClearHeat,	// クリア盛り上がり
	&CGallery::StateCounterHeat,	// カウンター盛り上がり
};

//==========================================================================
// コンストラクタ
//==========================================================================
CGallery::CGallery(int nPriority) : CObject(nPriority)
{
	// クリア
	m_State = STATE_NONE;	// 状態
	m_fStateTime = 0.0f;	// 状態カウンター
	m_GalleryInfo.clear();	// 観衆情報
}

//==========================================================================
// デストラクタ
//==========================================================================
CGallery::~CGallery()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CGallery* CGallery::Create(const MyLib::Vector3& pos, const MyLib::Vector3& rot)
{
	// 生成用のオブジェクト
	CGallery* pGallery = nullptr;

	// メモリの確保
	pGallery = DEBUG_NEW CGallery;

	if (pGallery != nullptr)
	{
		// 情報設定
		pGallery->SetOriginPosition(pos);
		pGallery->SetPosition(pos);
		pGallery->SetRotation(rot);

		// 初期化処理
		if (FAILED(pGallery->Init()))
		{
			return nullptr;
		}
	}

	return pGallery;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CGallery::Init()
{
	CXLoad* pXload = CXLoad::GetInstance();

	// モデルインデックス番号
	if (m_nModelIdx.empty())
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				m_nModelIdx.emplace_back();
				m_nModelIdx.back() = pXload->XLoad(FILENAME[i][j]);
			}
		}
	}

	// 種類の設定
	SetType(TYPE_OBJECTX);

	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();

	for (int height = 0; height < 3; height++)
	{
		for (int i = 0; i < NUM_GALLERY; i++)
		{
			// 生成時の位置
			MyLib::Vector3 spawnpos = pos;
			spawnpos.y += height * 40.0f;

			spawnpos.x = (pos.x + sinf(D3DX_PI * 0.5f + rot.y) * (i * DISTANCE)) - sinf(D3DX_PI * 0.5f + rot.y) * (DISTANCE * (NUM_GALLERY / 2));
			spawnpos.z = (pos.z + cosf(D3DX_PI * 0.5f + rot.y) * (i * DISTANCE)) - cosf(D3DX_PI * 0.5f + rot.y) * (DISTANCE * (NUM_GALLERY / 2));
			spawnpos.x += UtilFunc::Transformation::Random(-20, 20);
			spawnpos.z += UtilFunc::Transformation::Random(-20, 20);

			// 段の分ずらす
			spawnpos.x -= sinf(D3DX_PI + rot.y) * (height * DISTANCE_DEPTH);
			spawnpos.z -= cosf(D3DX_PI + rot.y) * (height * DISTANCE_DEPTH);

			// 生成時の向き
			MyLib::Vector3 spawnrot = rot;
			spawnrot.y += D3DXToRadian(UtilFunc::Transformation::Random(-10, 10));


			// 観衆生成
			m_GalleryInfo.emplace_back();
			m_GalleryInfo.back().colorType = static_cast<COLORTYPE>(UtilFunc::Transformation::Random(0, static_cast<int>(COLORTYPE_MAX - 1)));	// 色の種類
			m_GalleryInfo.back().objX = CObjectX::Create(FILENAME[m_GalleryInfo.back().colorType][0], spawnpos, spawnrot, false);

			if (m_GalleryInfo.back().objX == nullptr)
			{
				return E_FAIL;
			}
			m_GalleryInfo.back().objX->SetOriginPosition(spawnpos);

			// 種類設定
			m_GalleryInfo.back().objX->SetType(CObject::TYPE_OBJECTX);
		}
	}

	// リストに割り当て
	m_List.Regist(this);
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CGallery::Uninit()
{
	for (const auto& info : m_GalleryInfo)
	{
		info.objX->Uninit();
	}

	// クリア
	m_GalleryInfo.clear();

	// リストから削除
	m_List.Delete(this);

	// 情報削除
	Release();

	// インデックスリセット
	m_nModelIdx.clear();
}

//==========================================================================
// 削除処理
//==========================================================================
void CGallery::Kill()
{
	for (const auto& info : m_GalleryInfo)
	{
		info.objX->Kill();
	}

	// クリア
	m_GalleryInfo.clear();

	// リストから削除
	m_List.Delete(this);

	// 情報削除
	Release();
}


//==========================================================================
// 更新処理
//==========================================================================
void CGallery::Update()
{
	// 状態更新
	(this->*(m_StateFunc[m_State]))();
}

//==========================================================================
// クリア盛り上がり
//==========================================================================
void CGallery::StateClearHeat()
{
	// ジャンプ
	Jump();
}

//==========================================================================
// カウンター盛り上がり
//==========================================================================
void CGallery::StateCounterHeat()
{

	// 状態カウンター加算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	if (TIME_COUNTERHEAT <= m_fStateTime)
	{
		for (auto& info : m_GalleryInfo)
		{
			MyLib::Vector3 move = info.objX->GetMove();
			MyLib::Vector3 pos = info.objX->GetPosition();
			MyLib::Vector3 posorigin = info.objX->GetOriginPosition();

			// 位置更新
			pos += move;

			// 重力
			move.y -= GRAVITY;
			info.objX->SetMove(move);

			if (pos.y <= posorigin.y)
			{// 初期値を下回れば

				// 初期値固定
				pos.y = posorigin.y;
				info.objX->SetMove(0.0f);

				// 通常のポーズに戻す
				info.poseType = POSE_NONE;

				// モデル切り替え
				int color = info.colorType, pose = info.poseType;
				info.objX->BindXData(m_nModelIdx[3 * color + pose]);
			}

			info.objX->SetPosition(pos);
		}
	}
	else
	{
		// ジャンプ
		Jump();
	}
}

//==========================================================================
// ジャンプ
//==========================================================================
void CGallery::Jump()
{
	for (auto& info : m_GalleryInfo)
	{
		MyLib::Vector3 move = info.objX->GetMove();
		MyLib::Vector3 pos = info.objX->GetPosition();
		MyLib::Vector3 posorigin = info.objX->GetOriginPosition();

		// 位置更新
		pos += move;

		// 重力
		move.y -= GRAVITY;
		info.objX->SetMove(move);

		if (pos.y <= posorigin.y)
		{// 初期値を下回れば

			pos.y = posorigin.y;

			float jump = JUMP + UtilFunc::Transformation::Random(-20, 20) * 0.05f;
			info.objX->SetMove(MyLib::Vector3(0.0f, jump, 0.0f));

			info.poseType =
				(info.poseType == POSE_RIGHT) ? POSE_LEFT :
				(info.poseType == POSE_LEFT) ? POSE_RIGHT :
				static_cast<POSE>(UtilFunc::Transformation::Random(1, 2));

			// モデル切り替え
			int color = info.colorType, pose = info.poseType;
			info.objX->BindXData(m_nModelIdx[3 * color + pose]);
		}

		info.objX->SetPosition(pos);
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CGallery::Draw()
{
	

}

//==========================================================================
// 状態設定
//==========================================================================
void CGallery::SetState(STATE state)
{ 
	m_fStateTime = 0.0f;
	m_State = state;
}

//==========================================================================
// 観衆設定
//==========================================================================
void CGallery::SetGallery(void)
{
	char aComment[MAX_COMMENT] = {};	// コメント用
	int nTexNum = 0;					// ファイルの数
	int nCntTexture = 0;				// テクスチャ読み込みカウント

	//ファイルを開く
	FILE* pFile = fopen(LOADTEXT, "r");

	if (pFile == nullptr)
	{//ファイルが開けた場合
		return;
	}

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// モデルの設定
		if (strcmp(&aComment[0], "MODELSET") == 0)
		{// モデルの読み込みを開始

			MyLib::Vector3 pos = mylib_const::DEFAULT_VECTOR3;
			MyLib::Vector3 rot = mylib_const::DEFAULT_VECTOR3;

			while (strcmp(&aComment[0], "END_MODELSET"))
			{// END_MODELSETが来るまで繰り返し

				fscanf(pFile, "%s", &aComment[0]);	// 確認する

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

			// 生成
			CGallery::Create(pos, rot);
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける

			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);
}
