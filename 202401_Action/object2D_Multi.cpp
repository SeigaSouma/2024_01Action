//=============================================================================
// 
//  オブジェクト2D処理 [object2D_Multi.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "object2D_Multi.h"
#include "manager.h"
#include "renderer.h"
#include "player.h"
#include "enemy.h"
#include "bg.h"
#include "texture.h"
#include "pause.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE_NAME	"data\\TEXTURE\\titlelog_00.png"	// テクスチャ名
#define POLYGON_TOP		(4)								// ポリゴンの頂点数
#define WIDTH			(80.0f)							// 横幅
#define HEIGHT			(60.0f)							// 縦幅
#define ANIM_SPEED		(10)							// 読み込み間隔
#define MAX_PATTERN_U	(5)								// Uの分割数
#define MAX_PATTERN_V	(2)								// Vの分割数
#define MAX_PATTERN		(MAX_PATTERN_U * MAX_PATTERN_V)
#define MOVE_U			(1.0f / (float)MAX_PATTERN_U)	// U座標移動量
#define MOVE_V			(1.0f / (float)MAX_PATTERN_V)	// V座標移動量

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CObject2DMulti::CObject2DMulti(int nPriority) : CObject(nPriority)
{
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 色
	m_size = D3DXVECTOR2(0.0f, 0.0f);			// サイズ
	m_sizeOrigin = D3DXVECTOR2(0.0f, 0.0f);		// 元のサイズ
	m_fLength = 0.0f;							// 対角線の長さ
	m_fAngle = 0.0f;							// 対角線の向き
	m_pVtxBuff = NULL;							// 頂点バッファ
	m_nTexIdx = 0;								// テクスチャのインデックス番号

	for (int nCntVtx = 0; nCntVtx < 32; nCntVtx++)
	{
		m_fTex[nCntVtx] = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャ座標
		m_VtxPos[nCntVtx] = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// 頂点座標
	}

}

//==========================================================================
// デストラクタ
//==========================================================================
CObject2DMulti::~CObject2DMulti()
{

}

//==========================================================================
// テクスチャの割り当て
//==========================================================================
void CObject2DMulti::BindTexture(int nIdx)
{
	// 割り当てる
	m_nTexIdx = nIdx;
}

//==========================================================================
// 生成処理
//==========================================================================
CObject2DMulti *CObject2DMulti::Create()
{
	// 生成用のオブジェクト
	CObject2DMulti *pObject2D = NULL;

	if (pObject2D == NULL)
	{// NULLだったら

		// メモリの確保
		pObject2D = DEBUG_NEW CObject2DMulti;

		if (pObject2D != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pObject2D->Init();
		}

		return pObject2D;
	}

	return NULL;
}

//==========================================================================
// 生成処理
//==========================================================================
CObject2DMulti *CObject2DMulti::Create(int nPriority)
{
	// 生成用のオブジェクト
	CObject2DMulti *pObject2D = NULL;

	if (pObject2D == NULL)
	{// NULLだったら

		// メモリの確保
		pObject2D = DEBUG_NEW CObject2DMulti(nPriority);

		if (pObject2D != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pObject2D->Init();
		}

		return pObject2D;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CObject2DMulti::Init()
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	m_fTex[0] = D3DXVECTOR2(0.0f, 0.0f);	// テクスチャ座標
	m_fTex[1] = D3DXVECTOR2(1.0f, 0.0f);	// テクスチャ座標
	m_fTex[2] = D3DXVECTOR2(0.0f, 1.0f);	// テクスチャ座標
	m_fTex[3] = D3DXVECTOR2(1.0f, 1.0f);	// テクスチャ座標

	// 頂点バッファの生成
	if (m_pVtxBuff != NULL)
	{// 既に情報が入ってる場合

		return E_FAIL;
	}

	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_2D_MULTITEX) * POLYGON_TOP,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D_MULTITEX,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr))
	{// 失敗したとき

		return E_FAIL;
	}

	// 頂点情報設定
	SetVtx();

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CObject2DMulti::Uninit()
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// オブジェクトの破棄
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CObject2DMulti::Update()
{
	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CObject2DMulti::Draw()
{

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D_MULTITEX));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D_MULTITEX);

	// テクスチャの設定
	pDevice->SetTexture(0, CTexture::GetInstance()->GetAdress(m_nTexIdx));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void CObject2DMulti::Draw(LPDIRECT3DTEXTURE9 mutitex)
{

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// テクスチャステージ1の設定
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE); // ブレンディングモードをMODULATEに変更
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE); // アルファブレンディングもMODULATEに変更
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	// アルファブレンディングを有効にする
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D_MULTITEX));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D_MULTITEX);

	// テクスチャの設定
	pDevice->SetTexture(0, CTexture::GetInstance()->GetAdress(m_nTexIdx));
	pDevice->SetTexture(1, mutitex);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


	// テクスチャステージ0のデフォルト設定
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	// テクスチャステージ1のデフォルト設定
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE); // デフォルトではテクスチャステージ1は無効
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CObject2DMulti::SetVtx()
{

	// 頂点情報へのポインタ
	VERTEX_2D_MULTITEX *pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();
	D3DXCOLOR col = GetColor();
	float fAngle = GetAngle();
	float fLength = GetLength();

	// 頂点座標の設定
	pVtx[0].pos.x = pos.x + sinf(rot.z - D3DX_PI + fAngle) * fLength;
	pVtx[0].pos.y = pos.y + cosf(rot.z - D3DX_PI + fAngle) * fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = pos.x + sinf(rot.z + D3DX_PI - fAngle) * fLength;
	pVtx[1].pos.y = pos.y + cosf(rot.z + D3DX_PI - fAngle) * fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = pos.x + sinf(rot.z - fAngle) * fLength;
	pVtx[2].pos.y = pos.y + cosf(rot.z - fAngle) * fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = pos.x + sinf(rot.z + fAngle) * fLength;
	pVtx[3].pos.y = pos.y + cosf(rot.z + fAngle) * fLength;
	pVtx[3].pos.z = 0.0f;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// テクスチャ座標の設定
	pVtx[0].tex = m_fTex[0];
	pVtx[1].tex = m_fTex[1];
	pVtx[2].tex = m_fTex[2];
	pVtx[3].tex = m_fTex[3];

	// テクスチャ座標の設定
	pVtx[0].tex2 = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex2 = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex2 = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex2 = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックロック
	m_pVtxBuff->Unlock();
}

//==========================================================================
// 色設定
//==========================================================================
void CObject2DMulti::SetColor(const D3DXCOLOR col)
{
	m_col = col;
}

//==========================================================================
// 色取得
//==========================================================================
D3DXCOLOR CObject2DMulti::GetColor() const
{
	return m_col;
}

//==========================================================================
// サイズ設定
//==========================================================================
void CObject2DMulti::SetSize(const D3DXVECTOR2 size)
{
	m_size = size;		// サイズ
	SetLength(sqrtf(m_size.x * m_size.x + m_size.y * m_size.y));	// 対角線の長さ
	SetAngle(atan2f(m_size.x, m_size.y));								// 対角線の向き
}

//==========================================================================
// サイズ取得
//==========================================================================
D3DXVECTOR2 CObject2DMulti::GetSize() const
{
	return m_size;
}

//==========================================================================
// 元のサイズの設定
//==========================================================================
void CObject2DMulti::SetSizeOrigin(const D3DXVECTOR2 size)
{
	m_sizeOrigin = size;
}

//==========================================================================
// 元のサイズの取得
//==========================================================================
D3DXVECTOR2 CObject2DMulti::GetSizeOrigin() const
{
	return m_sizeOrigin;
}

//==========================================================================
// テクスチャ座標設定
//==========================================================================
void CObject2DMulti::SetTex(D3DXVECTOR2 *tex)
{
	memcpy(&m_fTex[0], tex, sizeof(m_fTex));
}

//==========================================================================
// テクスチャ座標取得
//==========================================================================
D3DXVECTOR2 *CObject2DMulti::GetTex()
{
	return &m_fTex[0];
}

//==========================================================================
// 対角線の長さ設定
//==========================================================================
void CObject2DMulti::SetLength(const float fLength)
{
	m_fLength = fLength;
}

//==========================================================================
// 対角線の長さ取得
//==========================================================================
float CObject2DMulti::GetLength() const
{
	return m_fLength;
}

//==========================================================================
// 対角線の向き取得
//==========================================================================
void CObject2DMulti::SetAngle(const float fAngle)
{
	m_fAngle = fAngle;
}

//==========================================================================
// 対角線の向き取得
//==========================================================================
float CObject2DMulti::GetAngle() const
{
	return m_fAngle;
}


//==========================================================================
// 頂点座標取得
//==========================================================================
MyLib::Vector3 *CObject2DMulti::GetVtxPos()
{
	return &m_VtxPos[0];
}

//==========================================================================
// 頂点座標設定
//==========================================================================
void CObject2DMulti::SetVtxPos(MyLib::Vector3 *pos)
{
	//&m_VtxPos[0] = *pos;
	memcpy(&m_VtxPos[0], pos, sizeof(m_VtxPos));
}

//==========================================================================
// 頂点バッファの取得
//==========================================================================
LPDIRECT3DVERTEXBUFFER9 CObject2DMulti::GetVtxBuff() const
{
	return m_pVtxBuff;
}

//==========================================================================
// オブジェクト2Dオブジェクトの取得
//==========================================================================
CObject2DMulti *CObject2DMulti::GetObject2DMulti()
{
	return this;
}
