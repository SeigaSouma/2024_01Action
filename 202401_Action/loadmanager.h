//=============================================================================
// 
// ���[�h�}�l�[�W�� [loadmanager.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _LOADMANAGER_H_
#define _LOADMANAGER_H_		// ��d�C���N���[�h�h�~

#include "scene.h"

class CLoadScreen;

//==========================================================================
// �N���X��`
//==========================================================================
// ���[�h�}�l�[�W���N���X
class CLoadManager
{
public:

	CLoadManager();
	~CLoadManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void LoadScene(void);
	void UnLoad(void);
	void ResetLoad();

	// ���[�h�������������ǂ�����Ԃ�
	bool IsLoadComplete()
	{
		std::lock_guard<std::mutex>  lock(isLoadedMutex);
		return isLoadComplete;
	}

	// �ÓI�֐�
	static CLoadManager* Create();

private:

	//=============================
	// �����o�֐�
	//=============================
	void LoadInBackground(void);
	void Load();

	// �V�����V�[�����Z�b�g����O�Ƀ��[�h�����Z�b�g��������֐�
	void ResetInternalLoad();

	//=============================
	// �����o�ϐ�
	//=============================
	std::thread m_LoadingThread;

	std::mutex isLoadedMutex;  // Mutex for isLoadComplete
	bool isLoadComplete = false;
	CLoadScreen* m_pLoadScreen;
};


#endif