//=============================================================================
// 
// スキルツリーリセット処理 [skilltree_reset.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_reset.h"
#include "skilltree.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "game.h"
#include "calculation.h"
#include "player.h"
#include "skillpoint.h"
#include "skilltree_reset_impossible.h"

//==========================================================================
// マクロ定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\skilltree\\resetwindow.png";
	const char* SELECT_TEXTURE[] =
	{
		"data\\TEXTURE\\skilltree\\reset_select01.png",
		"data\\TEXTURE\\skilltree\\reset_select02.png",
		"data\\TEXTURE\\skilltree\\reset_select03.png",
		"data\\TEXTURE\\skilltree\\reset_select04.png",
	};
	const float DISTANCE = 140.0f;
}
std::vector<int> CSkillTree_Reset::m_nTexIdx = {};		// テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Reset::CSkillTree_Reset(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_nSelect = 0;	// 選択肢
	m_fFlashTime = 0.0f;	// 点滅時間
	memset(m_pSelectObj, NULL, sizeof(m_pSelectObj));	// 選択肢のオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Reset::~CSkillTree_Reset()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Reset* CSkillTree_Reset::Create()
{
	// メモリの確保
	CSkillTree_Reset* pReset = DEBUG_NEW CSkillTree_Reset;

	if (pReset != nullptr)
	{
		// 初期化処理
		if (FAILED(pReset->Init()))
		{
			return nullptr;
		}
	}

	return pReset;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillTree_Reset::Init()
{
	// 初期化処理
	HRESULT hr = CObject2D::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	// 種類の設定
	SetType(TYPE_OBJECT2D);

	if (m_nTexIdx.empty())
	{
		for (const auto& tex : SELECT_TEXTURE)
		{
			// テクスチャ読み込み
			int nIdx = CTexture::GetInstance()->Regist(tex);
			m_nTexIdx.push_back(nIdx);
		}
	}

	// テクスチャの割り当て
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nIdx);

	// サイズ設定
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdx);
	size = UtilFunc::Transformation::AdjustSizeByHeight(size, 150.0f);
	SetSize(size);
	SetSizeOrigin(GetSize());

	// 位置設定
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// 選択肢生成
	CreateSelect();

	return S_OK;
}

//==========================================================================
// 選択肢生成
//==========================================================================
void CSkillTree_Reset::CreateSelect()
{
	for (int i = 0; i < SELECT_MAX; i++)
	{
		m_pSelectObj[i] = CObject2D::Create(GetPriority());

		// 種類の設定
		m_pSelectObj[i]->SetType(TYPE_OBJECT2D);

		// テクスチャ割り当て
		m_pSelectObj[i]->BindTexture(m_nTexIdx[i]);

		// サイズ設定
		D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(m_nTexIdx[i]);
		size = UtilFunc::Transformation::AdjustSizeByHeight(size, 32.0f);
		m_pSelectObj[i]->SetSize(size);
		m_pSelectObj[i]->SetSizeOrigin(GetSize());

		// 位置設定
		m_pSelectObj[i]->SetPosition(MyLib::Vector3((SCREEN_WIDTH * 0.5f) - DISTANCE + (DISTANCE * i) * 2.0f, 410.0f, 0.0f));
	}

	int selectIdx = static_cast<int>(SELECT_CANCEL);
	m_pSelectObj[selectIdx]->BindTexture(m_nTexIdx[selectIdx] + 2);

}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree_Reset::Uninit()
{
	// インデックス番号リセット
	m_nTexIdx.clear();

	for (int i = 0; i < SELECT_MAX; i++)
	{
		if (m_pSelectObj[i] == nullptr)
		{
			continue;
		}
		m_pSelectObj[i] = nullptr;
		m_pSelectObj[i] = nullptr;
	}

	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 削除処理
//==========================================================================
void CSkillTree_Reset::Kill()
{
	for (int i = 0; i < SELECT_MAX; i++)
	{
		if (m_pSelectObj[i] == nullptr)
		{
			continue;
		}
		m_pSelectObj[i]->Uninit();
		m_pSelectObj[i] = nullptr;
	}

	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillTree_Reset::Update()
{

	m_fFlashTime += CManager::GetInstance()->GetDeltaTime();	// 点滅時間

	// 選択肢更新
	UpdateSelect();
	if (IsDeath())
	{
		return;
	}

	for (int i = 0; i < SELECT_MAX; i++)
	{
		D3DXCOLOR col = m_pSelectObj[i]->GetColor();
		if (m_nSelect == i)
		{
			col = UtilFunc::Transformation::HSVtoRGB(0.0f, 0.0f, 0.7f + fabsf(sinf(D3DX_PI * (m_fFlashTime / 1.0f)) * 0.3f));
			m_pSelectObj[i]->BindTexture(m_nTexIdx[i] + 2);
		}
		else
		{
			// 黒っぽくする
			col = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
			m_pSelectObj[i]->BindTexture(m_nTexIdx[i]);
		}
		m_pSelectObj[i]->SetColor(col);
	}

	// 更新処理
	CObject2D::Update();
}

//==========================================================================
// 選択肢更新
//==========================================================================
void CSkillTree_Reset::UpdateSelect()
{
	// キーボード情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	if (pInputKeyboard->GetTrigger(DIK_D) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON_RIGHT, 0) ||
		(pInputGamepad->GetLStickTrigger(CInputGamepad::STICK_X) && pInputGamepad->GetStickMoveL(0).x > 0))
	{// 右

		// パターンNo.を更新
		m_nSelect = (m_nSelect + (SELECT_MAX - 1)) % SELECT_MAX;

		// サウンド再生
		//CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_CURSOR);
	}
	else if (pInputKeyboard->GetTrigger(DIK_A) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON_LEFT, 0) ||
		(pInputGamepad->GetLStickTrigger(CInputGamepad::STICK_X) && pInputGamepad->GetStickMoveL(0).x < 0))
	{// 左

		// パターンNo.を更新
		m_nSelect = (m_nSelect + 1) % SELECT_MAX;

		// サウンド再生
		//CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_CURSOR);
	}

	// 決定
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) ||
		pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		switch (m_nSelect)
		{
		case SELECT_CANCEL:
			CSkillTree::GetInstance()->OutReset();
			return;
			break;

		case SELECT_OK:

			// 習得状況取得
			std::vector<CSkillTree_Icon::eMastering> masterings = CSkillTree::GetInstance()->GetMastering();

			bool bReset = false;
			for (const auto& master : masterings)
			{
				// 1つでも習得済みがあればリセット
				if (master == CSkillTree_Icon::MASTERING_DONE)
				{
					bReset = true;
					break;
				}
			}

			// 未習得
			if (!bReset)
			{
				CSkillTree_ResetImpossible::Create(CSkillTree_ResetImpossible::ERRORTYPE_NOGET);
			}

			// 残りポイント取得
			CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
			int point = pPlayer->GetSkillPoint()->GetPoint();

			// ポイントないからリセット不可
			if (point == 0)
			{
				bReset = false;
				CSkillTree_ResetImpossible::Create(CSkillTree_ResetImpossible::ERRORTYPE_POINT);
				return;
			}

			if (bReset)
			{
				// ポイント消費
				pPlayer->GetSkillPoint()->SubPoint();
				int backPoint = pPlayer->GetSkillPoint()->GetPoint();

				// アイコン情報取得
				std::vector<CSkillTree_Icon::sSkillIcon> iconInfo = CSkillTree::GetInstance()->GetIconInfo();
				for (const auto& info : iconInfo)
				{
					if (info.mastering == CSkillTree_Icon::MASTERING_DONE)
					{
						backPoint += info.needpoint;
					}
				}
				pPlayer->GetSkillPoint()->SetPoint(backPoint);

				// 強化リセット
				pPlayer->ResetEnhance();

				// 習得状況リセット
				CSkillTree::GetInstance()->ResetMastering();
				CSkillTree::GetInstance()->OutReset();
				return;
			}

			break;
		}
	}

	// 戻る
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_B, 0) ||
		pInputKeyboard->GetTrigger(DIK_BACK))
	{
		CSkillTree::GetInstance()->OutReset();
		return;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CSkillTree_Reset::Draw()
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 描画処理
	CObject2D::Draw();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}
