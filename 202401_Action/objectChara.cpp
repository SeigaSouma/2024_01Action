//=============================================================================
// 
//  オブジェクトキャラクター処理 [objectChara.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "objectChara.h"
#include "calculation.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "3D_effect.h"

//==========================================================================
// コンストラクタ
//==========================================================================
CObjectChara::CObjectChara(int nPriority) : CObjectHierarchy(nPriority)
{
	// 値のクリア
	m_fVelocity = 0.0f;		// 移動速度
	m_fRotDest = 0.0f;		// 目標の向き
	m_nLife = 0;			// 体力
	m_nLifeOrigin = 0;		// 元の体力
	m_nMotionStartIdx = 0;	// モーション開始のインデックス番号
	m_nAddScore = 0;		// スコア加算量
	m_bInDicision = false;	// 攻撃判定中フラグ
}

//==========================================================================
// デストラクタ
//==========================================================================
CObjectChara::~CObjectChara()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CObjectChara* CObjectChara::Create(const std::string pTextFile)
{
	// メモリの確保
	CObjectChara* pObjChara = DEBUG_NEW CObjectChara;

	if (pObjChara != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		HRESULT hr = pObjChara->SetCharacter(pTextFile);
		if (FAILED(hr))
		{// 失敗していたら
			return nullptr;
		}
	}

	return pObjChara;
}

//==========================================================================
// キャラ作成
//==========================================================================
HRESULT CObjectChara::SetCharacter(const std::string pTextFile)
{
	// キャラ作成
	CObjectHierarchy::SetCharacter(pTextFile);

	// モーションの生成処理
	m_pMotion = CMotion::Create(pTextFile, this);

	// モーションの設定
	if (m_pMotion != nullptr)
	{
		m_pMotion->SetModel(GetModel(), GetNumModel());

		// ポーズのリセット
		m_pMotion->ResetPose(0);
	}

	// スフィアコライダーデータ読み込み
	LoadSphereColliders(pTextFile);

	return S_OK;
}

//==========================================================================
// オブジェクト毎のデータ割り当て
//==========================================================================
void CObjectChara::BindObjectData(int nCntData)
{

	// オブジェクト毎のデータ割り当て
	CObjectHierarchy::BindObjectData(nCntData);

	// 身長
	m_fHeight = m_aLoadData[nCntData].fHeight;

	// 移動速度
	m_fVelocity = m_aLoadData[nCntData].fVelocity;

	// 体力
	m_nLife = m_aLoadData[nCntData].nLife;
	m_nLifeOrigin = m_nLife;

	// スコア加算量
	m_nAddScore = m_aLoadData[nCntData].nAddScore;

	// モーションスタートのインデックス
	m_nMotionStartIdx = m_aLoadData[nCntData].nMotionStartIdx;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CObjectChara::Init()
{
	// 初期化処理
	CObjectHierarchy::Init();

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CObjectChara::Uninit()
{
	if (m_pMotion != nullptr)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = nullptr;
	}

	// 終了処理
	CObjectHierarchy::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CObjectChara::Update()
{
	// オブジェクトヒエラルキーの更新
	CObjectHierarchy::Update();

	// モーション更新
	if (m_pMotion != nullptr)
	{
		m_pMotion->Update();
	}

	// コライダーの位置調整
	for (auto& collider : m_SphereColliders)
	{
		// 判定するパーツ取得
		CModel* pModel = GetModel()[collider.nParentPartsIdx];

		// 判定するパーツのマトリックス取得
		D3DXMATRIX mtxTrans;
		D3DXMATRIX mtxWepon = pModel->GetWorldMtx();

		// 位置を反映する
		D3DXMatrixTranslation(&mtxTrans, collider.offset.x, collider.offset.y, collider.offset.z);
		D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);
		collider.center = UtilFunc::Transformation::WorldMtxChangeToPosition(mtxWepon);
	}

	// モーション中の行動処理
	MotionInProgressAction();
}

//==========================================================================
// モーション中の行動処理
//==========================================================================
void CObjectChara::MotionInProgressAction()
{
	// 攻撃判定フラグリセット
	m_bInDicision = false;

	if (m_pMotion == nullptr)
	{
		return;
	}

	// 情報取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());
	int nType = m_pMotion->GetType();

	// 攻撃情報の総数取得
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (aInfo.AttackInfo[nCntAttack] == nullptr)
		{
			continue;
		}

		// 攻撃情報取得
		CMotion::AttackInfo* AttackInfo = aInfo.AttackInfo[nCntAttack];

		if (m_pMotion->IsImpactFrame(*AttackInfo))
		{// 衝撃のカウントと同じとき

			// 攻撃時処理
			AttackAction(*AttackInfo, nCntAttack);
		}

		// モーションカウンター取得
		float fAllCount = m_pMotion->GetAllCount();
		if (fAllCount > AttackInfo->nMinCnt && fAllCount <= AttackInfo->nMaxCnt)
		{// 攻撃判定中

			// 攻撃判定フラグ
			m_bInDicision = true;

			// 攻撃判定中処理
			AttackInDicision(AttackInfo, nCntAttack);
		}
	}
}

//==========================================================================
// 攻撃時処理
//==========================================================================
void CObjectChara::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// 武器の位置
	MyLib::Vector3 weponpos = m_pMotion->GetAttackPosition(GetModel(), ATKInfo);
}

//==========================================================================
// 攻撃判定中処理
//==========================================================================
void CObjectChara::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
{
	// 武器の位置
	MyLib::Vector3 weponpos = m_pMotion->GetAttackPosition(GetModel(), *pATKInfo);
}

//==========================================================================
// モーションファイル変更
//==========================================================================
void CObjectChara::ChangeMotion(const char* pMotionFile)
{
	if (m_pMotion != nullptr)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = nullptr;
	}

	// モーションの生成処理
	m_pMotion = CMotion::Create(pMotionFile, this);

	// モーションの設定
	m_pMotion->SetModel(GetModel(), GetNumModel());

	// ポーズのリセット
	m_pMotion->ResetPose(0);
}

//==========================================================================
// 描画処理
//==========================================================================
void CObjectChara::Draw()
{
	// 描画処理
	CObjectHierarchy::Draw();
}

//==========================================================================
// 描画処理
//==========================================================================
void CObjectChara::Draw(D3DXCOLOR col)
{
	// 描画処理
	CObjectHierarchy::Draw(col);
}

//==========================================================================
// 描画処理
//==========================================================================
void CObjectChara::Draw(float fAlpha)
{
	// 描画処理
	CObjectHierarchy::Draw(fAlpha);
}

//==========================================================================
// オブジェクト毎のデータ読み込み
//==========================================================================
void CObjectChara::LoadObjectData(FILE* pFile, const char* pComment)
{
	// オブジェクト毎のデータ読み込み
	CObjectHierarchy::LoadObjectData(pFile, pComment);

	char hoge[MAX_COMMENT];	// コメント

	if (strcmp(pComment, "HEIGHT") == 0)
	{// HEIGHTで身長

		fscanf(pFile, "%s", &hoge[0]);	// =の分
		fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad].fHeight);	// 身長
		m_fHeight = m_aLoadData[m_nNumLoad].fHeight;
	}

	if (strcmp(pComment, "VELOCITY") == 0)
	{// VELOCITYで移動速度

		fscanf(pFile, "%s", &hoge[0]);	// =の分
		fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad].fVelocity);	// 移動速度
		m_fVelocity = m_aLoadData[m_nNumLoad].fVelocity;
	}

	if (strcmp(pComment, "LIFE") == 0)
	{// LIFEで体力

		fscanf(pFile, "%s", &hoge[0]);	// =の分
		fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad].nLife);	// 体力
		m_nLife = m_aLoadData[m_nNumLoad].nLife;
		m_nLifeOrigin = m_nLife;	// 元の体力
	}

	if (strcmp(pComment, "MOTION_STARTPARTS") == 0)
	{// MOTION_STARTPARTSでモーション開始のインデックス番号

		fscanf(pFile, "%s", &hoge[0]);	// =の分
		fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad].nMotionStartIdx);	// モーション開始のインデックス番号
		m_nMotionStartIdx = m_aLoadData[m_nNumLoad].nMotionStartIdx;
	}

	if (strcmp(pComment, "SCORE") == 0)
	{// LIFEで体力

		fscanf(pFile, "%s", &hoge[0]);	// =の分
		fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad].nAddScore);	// 体力
		m_nAddScore = m_aLoadData[m_nNumLoad].nAddScore;
	}

}

//==========================================================================
// パーツ毎のデータ読み込み
//==========================================================================
void CObjectChara::LoadPartsData(FILE* pFile, const char* pComment, int *pCntParts)
{
	// パーツ毎のデータ読み込み
	CObjectHierarchy::LoadPartsData(pFile, pComment, pCntParts);
}

//==========================================================================
// スフィアコライダー読み込み
//==========================================================================
void CObjectChara::LoadSphereColliders(const std::string textfile)
{
	// ファイルを開く
	FILE* pFile = fopen(textfile.c_str(), "r");
	if (pFile == nullptr)
	{//ファイルが開けなかった場合
		return;
	}

	std::string filename;

	// 読み込み用変数
	char aComment[MAX_COMMENT];	// コメント用

	while (1)
	{
		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// コライダーファイルの読み込み
		if (strcmp(aComment, "COLLIDER_FILENAME") == 0)
		{// COLLIDER_FILENAMEがきたら

			fscanf(pFile, "%s", &aComment[0]);	// =の分
			fscanf(pFile, "%s", &aComment[0]);	// ファイル名

			// ファイル名保存
			filename = aComment;
			break;
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける
			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);


	// ファイルからJSONを読み込む
	std::ifstream file(filename);
	if (!file.is_open()) 
	{
		return;
	}

	nlohmann::json jsonData;
	file >> jsonData;	// jsonデータを与える

	// jsonデータから読み込む
	from_json(jsonData);
}

//==========================================================================
// スフィアコライダー書き込み
//==========================================================================
void CObjectChara::SaveSphereColliders()
{
	// ファイルにキャラクターのデータを書き込む
	std::ofstream outFile("character_data.json");
	if (!outFile.is_open())
	{
		return;
	}

	nlohmann::json outputData;
	to_json(outputData);
}

//==========================================================================
// スフィアコライダーの数取得
//==========================================================================
int CObjectChara::GetSphereColliderNum()
{
	return static_cast<int>(m_SphereColliders.size());
}

//==========================================================================
// スフィアコライダー取得
//==========================================================================
std::vector<CObjectChara::SphereCollider> CObjectChara::GetSphereColliders()
{ 
	return m_SphereColliders; 
}

//==========================================================================
// コライダー取得
//==========================================================================
CObjectChara::SphereCollider CObjectChara::GetNowSphereCollider(int nIdx)
{
	if (nIdx >= 0 && nIdx < static_cast<int>(m_SphereColliders.size()))
	{
#if 0
		// 判定するパーツ取得
		CModel* pModel = GetModel()[m_SphereColliders[nIdx].nParentPartsIdx];

		// 判定するパーツのマトリックス取得
		D3DXMATRIX mtxTrans;
		D3DXMATRIX mtxWepon = pModel->GetWorldMtx();

		// 位置を反映する
		D3DXMatrixTranslation(&mtxTrans, m_SphereColliders[nIdx].offset.x, m_SphereColliders[nIdx].offset.y, m_SphereColliders[nIdx].offset.z);
		D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);
		m_SphereColliders[nIdx].center = UtilFunc::Transformation::WorldMtxChangeToPosition(mtxWepon);
#endif
		return m_SphereColliders[nIdx];
	}
	return SphereCollider();
}

//==========================================================================
// 身長設定
//==========================================================================
void CObjectChara::SetHeight(const float fHeight)
{
	m_fHeight = fHeight;
}

//==========================================================================
// 身長取得
//==========================================================================
float CObjectChara::GetHeight()
{
	return m_fHeight;
}

//==========================================================================
// 移動速度設定
//==========================================================================
void CObjectChara::SetVelocity(const float fVelocity)
{
	m_fVelocity = fVelocity;
}

//==========================================================================
// 移動速度設定
//==========================================================================
float CObjectChara::GetVelocity() const
{
	return m_fVelocity;
}

//==========================================================================
// 目標の向き設定
//==========================================================================
void CObjectChara::SetRotDest(const float fRotDest)
{
	m_fRotDest = fRotDest;
}

//==========================================================================
// 目標の向き設定
//==========================================================================
float CObjectChara::GetRotDest() const
{
	return m_fRotDest;
}

//==========================================================================
// 体力設定
//==========================================================================
void CObjectChara::SetLife(const int nLife)
{
	m_nLife = nLife;
}

//==========================================================================
// 体力取得
//==========================================================================
int CObjectChara::GetLife() const
{
	return m_nLife;
}

//==========================================================================
// 元の体力設定
//==========================================================================
void CObjectChara::SetLifeOrigin(const int nLife)
{
	m_nLifeOrigin = nLife;
}

//==========================================================================
// 元の体力取得
//==========================================================================
int CObjectChara::GetLifeOrigin() const
{
	return m_nLifeOrigin;
}

//==========================================================================
// モーション開始のインデックス番号取得
//==========================================================================
int CObjectChara::GetMotionStartIdx() const
{
	return m_nMotionStartIdx;
}

//==========================================================================
// スコア加算量取得
//==========================================================================
int CObjectChara::GetAddScoreValue() const
{
	return m_nAddScore;
}

//==========================================================================
// モーションオブジェクト取得
//==========================================================================
CMotion* CObjectChara::GetMotion()
{
	return m_pMotion;
}

//==========================================================================
// オブジェクトキャラクターオブジェクトの取得
//==========================================================================
CObjectChara* CObjectChara::GetObjectChara()
{
	return this;
}
