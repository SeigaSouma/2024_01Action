//==========================================
//
//  勝利メッセージ(message_win.cpp)
//  Author : Tomoya Kanazaki
//
//==========================================
#include "message_win.h"
#include "manager.h"
#include "texture.h"
#include "camera.h"
#include "calculation.h"

//==========================================
//  定数定義
//==========================================
namespace
{
	const char* TEXPASS[2] = // テクスチャファイルパス
	{
		"data\\TEXTURE\\result\\perfect.png",
		"data\\TEXTURE\\result\\win.png"
	};
	const MyLib::Vector3 DEF_POS = MyLib::Vector3(1000.0f, 0.0f, 100.0f); // カメラからの相対初期位置
	const MyLib::Vector3 DIFF_POS = MyLib::Vector3(51.2f, 0.0f, 150.0f); // カメラからの相対目的地
	const MyLib::Vector3 LOGO_SIZE = MyLib::Vector3(51.2f, 27.2f, 0.0f); //ポリゴンサイズ
	const float MOVE_SPEED = 5.0f; // (a * x) ^ nにおける定数a
}

//==========================================
//  静的メンバ変数宣言
//==========================================
int CMessageWin::nIdx = 0;

//==========================================
//  コンストラクタ
//==========================================
CMessageWin::CMessageWin() :
	m_fSec(0.0f),
	m_bMove(false),
	m_nIdx(nIdx)
{
	++nIdx;
}

//==========================================
//  デストラクタ
//==========================================
CMessageWin::~CMessageWin()
{
	--nIdx;
}

//==========================================
//  初期化処理
//==========================================
HRESULT CMessageWin::Init()
{
	// 初期化
	HRESULT hr = CObject3D::Init();

	// 種類を設定
	CObject::SetType(CObject::TYPE_OBJECT3D);

	// テクスチャの割り当て
	BindTexture(CTexture::GetInstance()->Regist(TEXPASS[m_nIdx]));

	// 位置を設定
	MyLib::Vector3 pos_Diff = DEF_POS;
	pos_target = DIFF_POS;
	if (m_nIdx == 0)
	{
		pos_Diff.x *= -1.0f;
		pos_target.x *= -1.0f;
	}
	pos_target += CManager::GetInstance()->GetCamera()->GetPositionV();
	SetPosition(CManager::GetInstance()->GetCamera()->GetPositionV() + pos_Diff);

	// サイズを設定
	SetSize(LOGO_SIZE);

	//値を返す
	return hr;
}

//==========================================
//  終了処理
//==========================================
void CMessageWin::Uninit()
{
	// 終了
	CObject3D::Uninit();
}

//==========================================
//  更新処理
//==========================================
void CMessageWin::Update()
{
	/// 移動
	if (!m_bMove)
	{
		Move();
	}

	// 更新
	CObject3D::Update();
}

//==========================================
//  描画処理
//==========================================
void CMessageWin::Draw()
{
	// 描画
	CObject3D::Draw();
}

//==========================================
//  生成処理
//==========================================
CMessageWin* CMessageWin::Create()
{
	// インスタンス生成
	CMessageWin* pLose = DEBUG_NEW CMessageWin;

	// nullptrチェック
	if (pLose == nullptr) { return nullptr; }

	// 初期化処理
	pLose->Init();

	// 値を返す
	return pLose;
}

//==========================================
//  移動
//==========================================
void CMessageWin::Move()
{
	// 経過時間を加算
	m_fSec += CManager::GetInstance()->GetDeltaTime() * MOVE_SPEED;

	// 位置を取得
	MyLib::Vector3 pos = GetPosition();

	// 位置を更新
	if (m_nIdx == 0)
	{
		pos.x += m_fSec * m_fSec;

		// 位置を補正
		if (pos_target.x < pos.x)
		{
			pos = pos_target;

			// 移動完了フラグをオン
			m_bMove = true;
		}
	}
	else
	{
		pos.x -= m_fSec * m_fSec;

		// 位置を補正
		if (pos_target.x > pos.x)
		{
			pos = pos_target;

			// 移動完了フラグをオン
			m_bMove = true;
		}
	}

	if (m_bMove)
	{
		CManager::GetInstance()->GetCamera()->SetShake(10, 10.0f, 0.0f);
	}

	// 位置を適用
	SetPosition(pos);
}
