//=============================================================================
// 
//  タイトル画面処理 [tutorial_screen.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "tutorial_screen.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"
#include "input.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH		(640.0f)	// 横幅
#define HEIGHT		(360.0f)	// 縦幅
#define ALPHATIME	(60)		// 不透明度更新の時間
#define LOGOMOVE_TIME	(180)	// ロゴ移動の時間
#define SLASH_TIME		(60)	// 斬撃の時間
#define SLASH_MOVETIME	(8)		// 斬撃の移動時間
#define LOGO_POSITION	(MyLib::Vector3(640.0f, 200.0f, 0.0f))
#define LOGOSIZE_MULTIPLY	(0.55f)
#define CHANGE_TIME		(40)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CTutorialScreen::m_apTextureFile[VTX_LOGO_MAX] =			// テクスチャのファイル
{
	"data\\TEXTURE\\tutorial.png",
};

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorialScreen::CTutorialScreen(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	memset(&m_pObj2D[0], 0, sizeof(m_pObj2D));				// オブジェクト2Dのオブジェクト
	memset(&m_nTexIdx[0], 0, sizeof(m_nTexIdx));				// テクスチャのインデックス番号
	m_bStick = false;		// スティックの判定
}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorialScreen::~CTutorialScreen()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTutorialScreen *CTutorialScreen::Create()
{
	// 生成用のオブジェクト
	CTutorialScreen *pTitleScreen = nullptr;

	if (pTitleScreen == nullptr)
	{// nullptrだったら

		// メモリの確保
		pTitleScreen = DEBUG_NEW CTutorialScreen;

		if (pTitleScreen != nullptr)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pTitleScreen->Init();
		}

		return pTitleScreen;
	}

	return nullptr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTutorialScreen::Init()
{
	// 種類の設定
	SetType(CObject::TYPE_OBJECT2D);

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		// 生成処理
		m_pObj2D[nCntSelect] = CObject2D::Create(8);

		// 種類の設定
		m_pObj2D[nCntSelect]->SetType(CObject::TYPE_OBJECT2D);

		// テクスチャの割り当て
		m_nTexIdx[nCntSelect] = CTexture::GetInstance()->Regist(m_apTextureFile[nCntSelect]);

		// テクスチャの割り当て
		m_pObj2D[nCntSelect]->BindTexture(m_nTexIdx[nCntSelect]);

		// サイズ設定
		m_pObj2D[nCntSelect]->SetSize(D3DXVECTOR2(640.0f, 360.0f));	// サイズ
		m_pObj2D[nCntSelect]->SetPosition(MyLib::Vector3(640.0f, 360.0f, 0.0f));	// 位置
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTutorialScreen::Uninit()
{
	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] != nullptr)
		{// nullptrじゃなかったら

			// 終了処理
			m_pObj2D[nCntSelect] = nullptr;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorialScreen::Update()
{

}

//==========================================================================
// 描画処理
//==========================================================================
void CTutorialScreen::Draw()
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		// 描画処理
		m_pObj2D[nCntSelect]->Draw();
	}
}

