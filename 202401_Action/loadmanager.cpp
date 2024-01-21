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
	m_bEndLoad = false;	// ���[�h�I��
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
	// ���b�N���Ĉ��S�Ƀ��Z�b�g�������s��
	//std::lock_guard<std::mutex> lock(isLoadedMutex);

	// ResetInternalLoad �֐����Ăяo���ă��[�h�����Z�b�g
	ResetInternalLoad();

}

void CLoadManager::ResetInternalLoad()
{
	// ���b�N���Ĉ��S�Ƀ��Z�b�g�������s��
	std::lock_guard<std::mutex> lock(isLoadedMutex);

	// �����ŕK�v�ȃ��\�[�X�̉����X���b�h�̏I�������Ȃǂ��s���܂�
	// �ȉ��͗�Ƃ��āA�X���b�h�̏I�������Ƃ��� join() ���g���Ă��܂����A
	// ���ۂ̃v���W�F�N�g�ł̓X���b�h�v�[���̊Ǘ��⃊�\�[�X�̉��������ǉ����Ă��������B

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
	// �t�F�[�h�擾
	CFade* fade = CManager::GetInstance()->GetFade();
	m_ModeNext = mode;
	m_bEndLoad = false;

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
	isLoadComplete = false;

	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}

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
	// �t�F�[�h�擾
	CFade* fade = CManager::GetInstance()->GetFade();

	// ���[�h���Ăюn�܂�̂Ńt���O�����Z�b�g
	{
		std::lock_guard<std::mutex> lock(isLoadedMutex);
		isLoadComplete = false;
	}

	while (1)
	{
		if (fade->GetState() == CFade::STATE_NONE)
		{
			break;
		}
	}

	try
	{
		Load();
	}
	catch (const std::exception& e)
	{
		return;
	}

	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}

	// �X���b�h�������I�������iWindows�p�j
	//TerminateThread(m_LoadingThread.native_handle(), 0);

	if (m_bEndLoad)
	{
		CManager::GetInstance()->GetInstantFade()->SetFade();
	}
	while (1)
	{
		if (CManager::GetInstance()->GetInstantFade()->GetState() == CInstantFade::STATE_FADECOMPLETION)
		{
			// ���[�h������������t���O���Z�b�g
			{
				std::lock_guard<std::mutex> lock(isLoadedMutex);
				isLoadComplete = true;
			}
			break;
		}
	}
}

//==========================================================================
// ���[�h����
//==========================================================================
void CLoadManager::Load()
{
	{
		std::lock_guard<std::mutex> lock(isLoadedMutex);

		// �V�[���̏���������
		CManager::GetInstance()->GetScene()->Init();
	}

	// ���[�h������������t���O���Z�b�g
	{
		std::lock_guard<std::mutex> lock(isLoadedMutex);
		m_bEndLoad = true;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CLoadManager::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// ���[�h���������Ă��Ȃ��ꍇ�̓f�o�C�X�A�N�Z�X�����b�N
	if (!isLoadComplete)
	{
		// ���b�N���g���ăX���b�h�Z�[�t�Ƀf�o�C�X�ɃA�N�Z�X
		std::lock_guard<std::mutex> lock(isLoadedMutex);

		// ��ʃN���A(�o�b�N�o�b�t�@��Z�o�b�t�@�̃N���A)
		pDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

		// �`��J�n
		if (SUCCEEDED(pDevice->BeginScene()))
		{
			if (m_pLoadScreen != nullptr)
			{
				m_pLoadScreen->Draw();
			}

			// �t�F�[�h�`�揈��
			CManager::GetInstance()->GetFade()->Draw();

			// �t�F�[�h�`�揈��
			CManager::GetInstance()->GetInstantFade()->Draw();

			// �`��I��
			pDevice->EndScene();
		}

		// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
		pDevice->Present(NULL, NULL, NULL, NULL);
	}
}

