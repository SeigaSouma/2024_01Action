//==========================================
//
//  敗北メッセージ(message_lose.cpp)
//  Author : Tomoya Kanazaki
//
//==========================================
#include "message_lose.h"
#include "manager.h"
#include "texture.h"
#include "camera.h"
#include "calculation.h"

//==========================================
//  定数定義
//==========================================
namespace
{
	const char* TEXPASS = "data\\TEXTURE\\result\\lose.png"; // テクスチャファイルパス
	const MyLib::Vector3 DEF_POS = MyLib::Vector3(0.0f, 0.0f, 2000.0f); // カメラからの相対初期位置
	const MyLib::Vector3 DIFF_POS = MyLib::Vector3(0.0f, 0.0f, 100.0f); // カメラからの相対目的地
	const MyLib::Vector3 LOGO_SIZE = MyLib::Vector3(64.0f, 32.0f, 0.0f); //ポリゴンサイズ
	const float MOVE_SPEED = 5.0f; // a * x ^ における定数a
}

//==========================================
//  コンストラクタ
//==========================================
CMessageLose::CMessageLose() :
	m_fSec(0.0f),
	m_bMove(false)
{

}

//==========================================
//  デストラクタ
//==========================================
CMessageLose::~CMessageLose()
{

}

//==========================================
//  初期化処理
//==========================================
HRESULT CMessageLose::Init()
{
	// 初期化
	HRESULT hr = CObject3D::Init();

	// 種類を設定
	CObject::SetType(CObject::TYPE_OBJECT3D);

	// テクスチャの割り当て
	BindTexture(CTexture::GetInstance()->Regist(TEXPASS));

	// ロゴの位置を設定
	SetPosition(CManager::GetInstance()->GetCamera()->GetPositionV() + DEF_POS);
	MyLib::Vector3 pos = GetPosition();

	// ロゴのサイズを設定
	SetSize(LOGO_SIZE);

	//値を返す
	return hr;
}

//==========================================
//  終了処理
//==========================================
void CMessageLose::Uninit()
{
	// 終了
	CObject3D::Uninit();
}

//==========================================
//  更新処理
//==========================================
void CMessageLose::Update()
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
void CMessageLose::Draw()
{
	// 描画
	CObject3D::Draw();
}

//==========================================
//  生成処理
//==========================================
CMessageLose* CMessageLose::Create()
{
	// インスタンス生成
	CMessageLose* pLose = DEBUG_NEW CMessageLose;

	// NULLチェック
	if (pLose == nullptr) { return nullptr; }

	// 初期化処理
	pLose->Init();

	// 値を返す
	return pLose;
}

//==========================================
//  移動
//==========================================
void CMessageLose::Move()
{
	// 経過時間を加算
	m_fSec += CManager::GetInstance()->GetDeltaTime() * MOVE_SPEED;

	// 位置を取得
	MyLib::Vector3 pos = GetPosition();

	// 位置を更新
	pos.z -= m_fSec * m_fSec;

	// 位置を補正
	if ((CManager::GetInstance()->GetCamera()->GetPositionV() + DIFF_POS).z > pos.z)
	{
		pos = CManager::GetInstance()->GetCamera()->GetPositionV() + DIFF_POS;

		// 移動完了フラグをオン
		m_bMove = true;
	}

	if (m_bMove)
	{
		CManager::GetInstance()->GetCamera()->SetShake(20, 10.0f, 0.0f);
	}

	// 位置を適用
	SetPosition(pos);
}
