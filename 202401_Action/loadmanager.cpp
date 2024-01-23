//=============================================================================
// 
//  ���[�h�}�l�[�W�� [loadmanager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "loadmanager.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "loadscreen.h"
#include "fade.h"
#include "instantfade.h"

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CLoadManager::CLoadManager()
{
	m_LoadingThread;
	m_pLoadScreen = nullptr;
	isLoadComplete = false;
	m_ModeNext = CScene::MODE_NONE;
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CLoadManager::~CLoadManager()
{
	// ���[�h�X���b�h���܂������Ă���Ȃ�ҋ@
	if (m_LoadingThread.joinable()) {
		m_LoadingThread.join();
	}
}

//==========================================================================
// ��������
//==========================================================================
CLoadManager* CLoadManager::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CLoadManager* pLoadManager = NULL;

	// �������̊m��
	pLoadManager = DEBUG_NEW CLoadManager;

	if (pLoadManager != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		pLoadManager->Init();
	}

	return pLoadManager;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CLoadManager::Init(void)
{
	// ���[�h�X���b�h���܂������Ă���Ȃ�ҋ@
	if (m_LoadingThread.joinable()) {
		m_LoadingThread.join();
	}
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CLoadManager::Uninit(void)
{
	
}

//==========================================================================
// �X�V����
//==========================================================================
void CLoadManager::Update(void)
{
	if (m_pLoadScreen != nullptr)
	{
		m_pLoadScreen->Update();
	}

}

void CLoadManager::UnLoad(void)
{
	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}
}

void CLoadManager::ResetLoad()
{
	// ResetInternalLoad �֐����Ăяo���ă��[�h�����Z�b�g
	ResetInternalLoad();

}

void CLoadManager::ResetInternalLoad()
{
	// ���b�N���Ĉ��S�Ƀ��Z�b�g�������s��
	//std::lock_guard<std::mutex> lock(isLoadedMutex);

	// �X���b�h�����쒆�Ȃ�I����҂�
	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}

	// ���[�h���������Ă��Ȃ����Ƃ������t���O�����Z�b�g
	isLoadComplete = false;
}

//==========================================================================
// �V�[���̃��[�h���J�n
//==========================================================================
void CLoadManager::LoadScene(CScene::MODE mode)
{
	m_ModeNext = mode;

	if (m_pLoadScreen == nullptr)
	{
		m_pLoadScreen = CLoadScreen::Create();
	}

	if (m_LoadingThread.joinable())
	{
		// �f�^�b�`����O�ɃX���b�h����������܂őҋ@
		m_LoadingThread.join();
	}

	// ResetLoad �֐����Ăяo���ĐV�����V�[���̃��[�h������
	ResetLoad();

	// ���[�h���Ăюn�܂�̂Ńt���O�����Z�b�g
	{
		//std::lock_guard<std::mutex> lock(isLoadedMutex);
		//isLoadComplete = false;
	}
	isLoadComplete = false;

    // ���[�h�����̊J�n
	m_LoadingThread = std::thread(&CLoadManager::LoadInBackground, this);

	// �T�u�X���b�h���f�^�b�`
	m_LoadingThread.detach();
}

//==========================================================================
// ���[�h��ʂ�񓯊��ŕ\��
//==========================================================================
void CLoadManager::LoadInBackground(void)
{
	// ���[�h���Ăюn�܂�̂Ńt���O�����Z�b�g
	{
		//std::lock_guard<std::mutex> lock(isLoadedMutex);
		//isLoadComplete = false;
	}
	isLoadComplete = false;

	try
	{
		// �ǂݍ��ݏ���
		Load();
	}
	catch (const std::exception& e)
	{// ��O
		return;
	}

	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}

	// ���[�h������������t���O���Z�b�g
	{
		//std::lock_guard<std::mutex> lock(isLoadedMutex);
		//isLoadComplete = true;
	}
	isLoadComplete = true;
}

//==========================================================================
// ���[�h����
//==========================================================================
void CLoadManager::Load()
{
	
	// �V�[���̏���������
	CManager::GetInstance()->GetScene()->Init();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CLoadManager::Draw(void)
{
	if (m_pLoadScreen != nullptr)
	{
		m_pLoadScreen->Draw();
	}
}

//==========================================================================
// ���[�h�������������ǂ�����Ԃ�
//==========================================================================
bool CLoadManager::IsLoadComplete()
{
	//std::lock_guard<std::mutex>  lock(isLoadedMutex);
	return isLoadComplete;
}