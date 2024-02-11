//=============================================================================
// 
//  �}�l�[�W���w�b�_�[ [manager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _MANAGER_H_
#define _MANAGER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"
#include "scene.h"

// �}�N����`
#define MOVE_LRUP			(0.75f)	// �΂ߏ�ړ�
#define MOVE_LR				(0.5f)	// �����ړ�
#define MOVE_LRDW			(0.25f)	// �΂ߏ�ړ�

// �O���錾
class CRenderer;
class CLight;
class CCamera;
class CInputKeyboard;
class CInputGamepad;
class CInputMouse;
class CSound;
class CTexture;
class CDebugProc;
class CPlayer;
class CMap;
class CXLoad;
class CEdit;
class CElevation;
class CEnemyManager;
class CFade;
class CInstantFade;
class CBlackFrame;
class CPause;
class CPowerGauge;
class CResultManager;
class CRankingManager;

//==========================================================================
// �N���X��`
//==========================================================================
// �}�l�[�W���N���X��`
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	void Uninit();
	void Update();
	void Draw();
	void Load();// �ǂݍ���

	static CManager *Create();		// ��������
	static CManager *GetInstance();	// �C���X�^���X�擾

	CRenderer *GetRenderer();
	CInputKeyboard *GetInputKeyboard();
	CInputGamepad *GetInputGamepad();
	CInputMouse *GetInputMouse();
	CSound *GetSound();
	CDebugProc *GetDebugProc();
	CLight *GetLight();
	CCamera *GetCamera();
	CEdit *GetEdit();
	CFade *GetFade();
	CInstantFade *GetInstantFade();	// �J�ڂȂ��t�F�[�h�擾
	CBlackFrame *GetBlackFrame();	// ���t���[���擾
	CPause *GetPause();
	CResultManager *GetResultManager();	// ���U���g�}�l�[�W���擾
	CRankingManager *GetRankingManager();	// �����L���O�}�l�[�W���̃I�u�W�F�N�g
	CScene *GetScene();		// �V�[���擾

	int GetNumPlayer();			// �v���C���[�̐��擾
	void SetNumPlayer(int nNum);	// �v���C���[�̐��ݒ�
	float GetDeltaTime();								// �o�ߎ��Ԏ擾
	void SetMode(CScene::MODE mode);					// ���̃��[�h�ݒ�
	CScene::MODE GetMode();							// ���݂̃��[�h�擾
	void SetEnableHitStop(int nCntHitStop);				// �q�b�g�X�g�b�v�̐ݒ�
	bool IsWireframe() { return m_bWireframe; }		// ���C���[�t���[��
	bool IsHitStop() { return m_bHitStop; }			// ���C���[�t���[��
	CScene::MODE GetOldMode() { return m_OldMode; }	// �O��̃��[�h�擾
	bool IsLoadComplete() { return m_bLoadComplete; }
private:

	void Reset(CScene::MODE mode);
	void NoLoadSetMode(CScene::MODE mode);		// ���̃��[�h�ݒ�

	CRenderer *m_pRenderer;				// �����_���[�̃I�u�W�F�N�g
	CInputKeyboard *m_pInputKeyboard;	// �L�[�{�[�h�̃I�u�W�F�N�g
	CInputGamepad *m_pInputGamepad;		// �Q�[���p�b�h�̃I�u�W�F�N�g
	CInputMouse *m_pInputMouse;			// �}�E�X�̃I�u�W�F�N�g
	CSound *m_pSound;					// �T�E���h�̃I�u�W�F�N�g
	CDebugProc *m_pDebugProc;			// �f�o�b�O�\���̃I�u�W�F�N�g
	CLight *m_pLight;					// ���C�g�̃I�u�W�F�N�g
	CCamera *m_pCamera;					// �J�����̃I�u�W�F�N�g
	CTexture *m_pTexture;				// �e�N�X�`���̃I�u�W�F�N�g
	CEdit *m_pEdit;						// �G�f�B�b�g�̃I�u�W�F�N�g
	CScene *m_pScene;					// �V�[���̃I�u�W�F�N�g
	CFade *m_pFade;						// �t�F�[�h�̃I�u�W�F�N�g
	CInstantFade *m_pInstantFade;		// �J�ڂȂ��t�F�[�h�̃I�u�W�F�N�g
	CBlackFrame *m_pBlackFrame;			// ���t���[���̃I�u�W�F�N�g
	CPause *m_pPause;					// �|�[�Y�̃I�u�W�F�N�g
	CResultManager *m_pResultManager;	// ���U���g�}�l�[�W���̃I�u�W�F�N�g
	CRankingManager *m_pRankingManager;	// �����L���O�}�l�[�W���̃I�u�W�F�N�g
	CMyEffekseer* m_pMyEffekseer;		// �G�t�F�N�V�A�̃I�u�W�F�N�g

	bool m_bWireframe;					// ���C���[�t���[��
	bool m_bHitStop;					// �q�b�g�X�g�b�v�̔���
	int m_nCntHitStop;					// �q�b�g�X�g�b�v�̃J�E���^�[
	CScene::MODE m_OldMode;				// �O��̃��[�h
	DWORD m_CurrentTime;				// ���ݎ���
	DWORD m_OldTime;					// �ߋ��̎���
	float m_fDeltaTime;					// �o�ߎ���
	int m_nNumPlayer;					// �v���C���[�̐�
	float m_fLoadTimer;					// ���[�h�̃^�C�}�[
	bool m_bLoadComplete;				// ���[�h�����̃t���O
	bool m_bLoadFadeSet;				// ���[�h�̃t�F�[�h�ݒ�t���O
	bool m_bNowLoading;				// ���[�h�����̃t���O
	bool m_bFirstLoad;				// ���񃍁[�h

	static CManager *m_pManager;	// �}�l�[�W���̃I�u�W�F�N�g
};


#endif