//=============================================================================
// 
// スキルツリーカーソル処理 [skilltree_cursor.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_cursor.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "fade.h"


//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\tyuuni\\tyuuni_face.png";
	const float MOVE_VELOCITY = 50.0f;	// 移動速度
}

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Cursor::CSkillTree_Cursor(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_nMyPlayerIdx = 0;	// プレイヤーインデックス番号
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Cursor::~CSkillTree_Cursor()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Cursor *CSkillTree_Cursor::Create(int nIdx)
{
	// 生成用のオブジェクト
	CSkillTree_Cursor *pEffect = NULL;

	if (pEffect == NULL)
	{// NULLだったら

		// メモリの確保
		pEffect = DEBUG_NEW CSkillTree_Cursor;

		if (pEffect != NULL)
		{// メモリの確保が出来ていたら

			// プレイヤーインデックス
			pEffect->m_nMyPlayerIdx = nIdx;

			// 初期化処理
			pEffect->Init();
		}

		return pEffect;
	}

	return NULL;
}

//==================================================================================
// 初期化処理
//==================================================================================
HRESULT CSkillTree_Cursor::Init(void)
{
	HRESULT hr;

	// 初期化処理
	hr = CObject2D::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	// 種類の設定
	SetType(TYPE_OBJECT2D);

	// テクスチャの割り当て
	int nIdxTex = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nIdxTex);

	// テクスチャサイズ取得
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdxTex) * 0.1f;

	// サイズ設定
	SetSize(size);

	return S_OK;
}

//==================================================================================
// 終了処理
//==================================================================================
void CSkillTree_Cursor::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//==================================================================================
// 更新処理
//==================================================================================
void CSkillTree_Cursor::Update(void)
{
	CManager* pManager = CManager::GetInstance();
	float deltatime = pManager->GetDeltaTime();

	// 移動速度
	float velocity = MOVE_VELOCITY * deltatime;

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = pManager->GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = pManager->GetInputGamepad();

	if (pInputKeyboard->GetPress(DIK_A) == true)
	{//←キーが押された,左移動

		if (pInputKeyboard->GetPress(DIK_W) == true)
		{//A+W,左上移動
			move.x += sinf(-D3DX_PI * 0.75f) * velocity;
			move.y += cosf(-D3DX_PI * 0.75f) * velocity;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{//A+S,左下移動
			move.x += sinf(-D3DX_PI * 0.25f) * velocity;
			move.y += cosf(-D3DX_PI * 0.25f) * velocity;
		}
		else
		{//A,左移動
			move.x += sinf(-D3DX_PI * 0.5f) * velocity;
			move.y += cosf(-D3DX_PI * 0.5f) * velocity;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_D) == true)
	{//Dキーが押された,右移動

		if (pInputKeyboard->GetPress(DIK_W) == true)
		{//D+W,右上移動
			move.x += sinf(D3DX_PI * 0.75f) * velocity;
			move.y += cosf(D3DX_PI * 0.75f) * velocity;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{//D+S,右下移動
			move.x += sinf(D3DX_PI * 0.25f) * velocity;
			move.y += cosf(D3DX_PI * 0.25f) * velocity;
		}
		else
		{//D,右移動
			move.x += sinf(D3DX_PI * 0.5f) * velocity;
			move.y += cosf(D3DX_PI * 0.5f) * velocity;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_W) == true)
	{//Wが押された、上移動
		move.x += sinf(D3DX_PI * 1.0f) * velocity;
		move.y += cosf(D3DX_PI * 1.0f) * velocity;
	}
	else if (pInputKeyboard->GetPress(DIK_S) == true)
	{//Sが押された、下移動
		move.x += sinf(D3DX_PI * 0.0f) * velocity;
		move.y += cosf(D3DX_PI * 0.0f) * velocity;
	}

	if (pInputGamepad->IsTipStick())
	{// 左スティックが倒れてる場合

		// スティックの向き取得
		float stickrot = pInputGamepad->GetStickRotL(m_nMyPlayerIdx);

		// 移動量と向き設定
		move.x += sinf(stickrot) * velocity;
		move.y += cosf(stickrot) * -velocity;
	}

	// 移動
	pos += move;

	// 慣性
	move.x += (0.0f - move.x) * 0.2f;
	move.y += (0.0f - move.y) * 0.2f;

	// 位置・移動量
	SetPosition(pos);
	SetMove(move);

	// 頂点座標の設定
	SetVtx();
}

//==================================================================================
// 描画処理
//==================================================================================
void CSkillTree_Cursor::Draw(void)
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

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CSkillTree_Cursor::SetVtx(void)
{
	// 頂点設定
	CObject2D::SetVtx();
}

