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
CDeadManager* CDeadManager::GetInstance(void)
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
CDeadManager* CDeadManager::Create(void)
{
	if (m_pThisPtr == NULL)
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
HRESULT CDeadManager::Init(void)
{
	
	// ��ނ̐ݒ�
	SetType(TYPE_OBJECT2D);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CDeadManager::Uninit(void)
{
	// �J������
	Release();
	m_pThisPtr = nullptr;
}

//==========================================================================
// �X�V����
//==========================================================================
void CDeadManager::Update(void)
{
	// ���ԉ��Z
	m_fTime += CManager::GetInstance()->GetDeltaTime();

	// �X�N���[���ݒ�
	if (TIME_SET_SCREEN <= m_fTime &&
		!m_bSetScreen)
	{
		CDeadScreen::Create();
		m_bSetScreen = true;
	}

	// �e�L�X�g�ݒ�
	if (TIME_SET_TEXT <= m_fTime &&
		!m_bSetText)
	{
		CDeadText::Create(TIME_FADE_TEXT);
		m_bSetText = true;
	}

	// �����m���ݒ�
	if (TIME_SET_REVIVALPERCENT_TEXT <= m_fTime &&
		!m_bSetRevivalText)
	{
		CRevivalPercentText::Create(TIME_FADE_REVIVALPERCENT_TEXT);
		m_bSetRevivalText = true;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CDeadManager::Draw(void)
{

}
