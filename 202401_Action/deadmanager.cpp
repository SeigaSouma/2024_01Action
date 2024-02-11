//=============================================================================
// 
// ���S�}�l�[�W���g���� [deadmanager.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "deadmanager.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "calculation.h"
#include "deadscreen.h"
#include "deadtext.h"
#include "revivalpercent_text.h"
#include "player.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const float TIME_SET_SCREEN = static_cast<float>(1) / static_cast<float>(mylib_const::DEFAULT_FPS);	// �X�N���[���̐ݒ莞��
	const float TIME_SET_TEXT = static_cast<float>(40) / static_cast<float>(mylib_const::DEFAULT_FPS);	// �e�L�X�g�̐ݒ莞��
	const float TIME_SET_REVIVALPERCENT_TEXT = 2.0f;	// �����m�������̐ݒ莞��
	const float TIME_FADE_TEXT = static_cast<float>(80) / static_cast<float>(mylib_const::DEFAULT_FPS);	// �e�L�X�g�̃t�F�[�h�ɂ����鎞��
	const float TIME_FADE_REVIVALPERCENT_TEXT = static_cast<float>(40) / static_cast<float>(mylib_const::DEFAULT_FPS);	// �����m�������̃t�F�[�h�ɂ����鎞��
}
CDeadManager* CDeadManager::m_pThisPtr = nullptr;	// ���g�̃|�C���^

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CDeadManager::CDeadManager(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_fTime = 0.0f;				// �J�E���^�[
	m_bSetScreen = false;		// �X�N���[���̃Z�b�g�t���O
	m_bSetText = false;			// �e�L�X�g�̃Z�b�g�t���O
	m_bSetRevivalText = false;	// �����m���e�L�X�g�̃Z�b�g�t���O
	m_bRespawnPossible = false;	// �����ł��邩
	m_pScreen = nullptr;		// �X�N���[���̃I�u�W�F
	m_pText = nullptr;			// �����̃I�u�W�F�N�g
	m_pRespawnPercent = nullptr;	// ���X�|�[���m��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CDeadManager::~CDeadManager()
{

}

//==========================================================================
// �C���X�^���X�擾
//==========================================================================
CDeadManager* CDeadManager::GetInstance()
{
	if (m_pThisPtr == nullptr)
	{
		m_pThisPtr = Create();
	}
	return m_pThisPtr;
}

//==========================================================================
// ��������
//==========================================================================
CDeadManager* CDeadManager::Create()
{
	if (m_pThisPtr == nullptr)
	{// �܂��������Ă��Ȃ�������

		// �C���X�^���X����
		m_pThisPtr = DEBUG_NEW CDeadManager;
	}

	// ����������
	m_pThisPtr->Init();
	return m_pThisPtr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CDeadManager::Init()
{
	// ��ނ̐ݒ�
	SetType(TYPE_OBJECT2D);

	m_fTime = 0.0f;	// �J�E���^�[���Z�b�g

	// ���X�|�[�����I
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
	m_bRespawnPossible = DrawingRevival(pPlayer->GetRespawnPercent());

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CDeadManager::Uninit()
{
	// �J������
	Release();
	m_pThisPtr = nullptr;
	m_pScreen = nullptr;			// �X�N���[���̃I�u�W�F
	m_pText = nullptr;				// �����̃I�u�W�F�N�g
	m_pRespawnPercent = nullptr;	// ���X�|�[���m��
}

//==========================================================================
// �X�V����
//==========================================================================
void CDeadManager::Update()
{
	// ���ԉ��Z
	m_fTime += CManager::GetInstance()->GetDeltaTime();

	// �X�N���[���ݒ�
	if (TIME_SET_SCREEN <= m_fTime &&
		!m_bSetScreen)
	{
		m_pScreen = CDeadScreen::Create(TIME_FADE_REVIVALPERCENT_TEXT);

		m_bSetScreen = true;
	}

	// �e�L�X�g�ݒ�
	if (TIME_SET_TEXT <= m_fTime &&
		!m_bSetText)
	{
		m_pText = CDeadText::Create(TIME_FADE_TEXT, TIME_FADE_REVIVALPERCENT_TEXT);
		m_bSetText = true;
	}

	// �����m���ݒ�
	if (TIME_SET_REVIVALPERCENT_TEXT <= m_fTime &&
		!m_bSetRevivalText)
	{
		CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
		m_pRespawnPercent = CRevivalPercentText::Create(pPlayer->GetRespawnPercent(), TIME_FADE_REVIVALPERCENT_TEXT);
		m_bSetRevivalText = true;
	}
}

//==========================================================================
// �t�F�[�h�A�E�g�ݒ�
//==========================================================================
void CDeadManager::SetFadeOut()
{
	m_pScreen->SetState(CDeadScreen::STATE_FADEOUT);		// �X�N���[���̃I�u�W�F
	m_pText->SetState(CDeadText::STATE_FADEOUT);			// �����̃I�u�W�F�N�g
	m_pRespawnPercent->SetState(CRevivalPercentText::STATE_FADEOUT);	// ���X�|�[���m��
}

//==========================================================================
// �������I
//==========================================================================
bool CDeadManager::DrawingRevival(int winningPercent)
{
	return UtilFunc::Transformation::Random(0, 99) < winningPercent;
}

//==========================================================================
// �`�揈��
//==========================================================================
void CDeadManager::Draw()
{

}
