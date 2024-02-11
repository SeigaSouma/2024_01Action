//=============================================================================
// 
//  �V�[���w�b�_�[ [scene.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SCENE_H_
#define _SCENE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "elevation.h"

//==========================================================================
// �O���錾
//==========================================================================
class CXLoad;
class CCamera;

//==========================================================================
// �N���X��`
//==========================================================================
// �V�[���N���X��`
class CScene
{
public:
	// ���[�h�̗񋓌^��`
	enum MODE
	{
		MODE_NONE = 0,		// �Ȃ�
		MODE_TITLE,			// �^�C�g��
		MODE_TUTORIAL,		// �`���[�g���A��
		MODE_GAME,			// �Q�[��
		MODE_RESULT,		// ���U���g
		MODE_RANKING,		// �����L���O
		MODE_MAX
	};

	CScene();
	~CScene();

	// �I�[�o�[���C�h����֐�
	virtual HRESULT Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();

	void ResetScene();	// �V�[���̃��Z�b�g
	static CScene *Create(MODE mode);
	static CXLoad *GetXLoad();			// X�t�@�C���̃f�[�^�擾
	static CElevation *GetElevation();	// �I�u�W�F�N�g3D�̎擾
	MODE GetMode();						// ���݂̃��[�h�擾

protected:

private:

	MODE m_mode;				// ���݂̃��[�h
	static CXLoad *m_pXLoad;						// X�t�@�C���̃I�u�W�F�N�g
	static CElevation *m_pObject3DMesh;				// �I�u�W�F�N�g3D���b�V���̃I�u�W�F�N�g
};



#endif