//=============================================================================
// 
//  �|�[�Y�w�b�_�[ [pause.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _PAUSE_H_
#define _PAUSE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object2D.h"

//==========================================================================
// �O���錾
//==========================================================================

//==========================================================================
// �N���X��`
//==========================================================================
// �|�[�Y�N���X��`
class CPause
{
public:

	CPause();
	~CPause();

	// �����o�֐�
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	void SetPause();	// �g�p�󋵕ύX
	bool IsPause();		// �g�p�󋵎擾

	static CPause *Create();	// ����

private:

	// �񋓌^��`
	enum VTX
	{
		VTX_FADE = 0,		// ������
		VTX_WINDOW,			// ���j���[�E�B���h�E
		VTX_CONTINUE,		// �Q�[���ɖ߂�
		VTX_RETRY,			// ���g���C
		VTX_QUIT,			// �^�C�g���ɖ߂�	
		VTX_MAX
	};

	// ���j���[
	enum MENU
	{
		MENU_RETURNGAME = 0,	// �|�[�Y�̉���
		MENU_RETRY,				// ���g���C
		MENU_RETURNTITLE,		// �^�C�g���ɖ߂�	
		MENU_MAX
	};

	void UpdateSelect();	// �I�����̍X�V

	CObject2D *m_aObject2D[VTX_MAX];				// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
	bool m_bPause;									// �|�[�Y�̔���
	D3DXCOLOR m_col;								// �|�[�Y�̃J���[
	int m_nSelect;									// �I����
	float m_fFlashTime;			// �_�Ŏ���
	static const char *m_apTextureFile[VTX_MAX];	// �e�N�X�`���̃t�@�C��
};

#endif