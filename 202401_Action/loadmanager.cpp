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

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CLoadManager::CLoadManager()
{
	m_LoadingThread;
	m_pLoadScreen = nullptr;
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

	// �����Ń��\�[�X�̉���Ȃǂ��s��
	// ...

	// ���[�h���������Ă��Ȃ����Ƃ������t���O�����Z�b�g
	isLoadComplete = false;
}

//==========================================================================
// �V�[���̃��[�h���J�n
//==========================================================================
void CLoadManager::LoadScene(void)
{
	if (m_pLoadScreen == nullptr)
	{
		m_pLoadScreen = CLoadScreen::Create();
	}

	// ResetLoad �֐����Ăяo���ĐV�����V�[���̃��[�h������
	ResetLoad();

	// ���[�h���Ăюn�܂�̂Ńt���O�����Z�b�g
	isLoadComplete = false;

	// ���[�h��ʂ�\��
	//std::cout << "Loading..." << std::endl;

	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}

	auto count = std::thread::hardware_concurrency();
	if (count < 3)
	{

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
	// ���[�h���Ăюn�܂�̂Ńt���O�����Z�b�g
	{
		std::lock_guard<std::mutex> lock(isLoadedMutex);
		isLoadComplete = false;
	}

	try
	{
		Load();
	}
	catch (const std::exception& e)
	{
		// ��O�����������ꍇ�͂����ŃG���[���O���o�͂Ȃǂ̓K�؂ȏ������s��
		std::cerr << "LoadInBackground: Exception caught: " << e.what() << std::endl;
	}

	// �X���b�h�������I�������iWindows�p�j
	TerminateThread(m_LoadingThread.native_handle(), 0);

	// ���[�h������������t���O���Z�b�g
	{
		std::lock_guard<std::mutex> lock(isLoadedMutex);
		isLoadComplete = true;
	}
}

//==========================================================================
// ���[�h����
//==========================================================================
void CLoadManager::Load()
{
	// �V�[���̏���������
	CManager::GetInstance()->GetScene()->Init();


	// ���[�h������������t���O���Z�b�g
	isLoadComplete = true;
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
			m_pLoadScreen->Update();
			m_pLoadScreen->Draw();

			// �`��I��
			pDevice->EndScene();
		}

		// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
		pDevice->Present(NULL, NULL, NULL, NULL);
	}

}

