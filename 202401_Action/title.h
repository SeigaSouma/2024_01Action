//=============================================================================
// 
//  �^�C�g���w�b�_�[ [title.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TITLE_H_
#define _TITLE_H_	// ��d�C���N���[�h�h�~

#include "manager.h"

class CTitleLogo;
class CTitle_PressEnter;

//==========================================================================
// �O���錾
//==========================================================================

//==========================================================================
// �N���X��`
//==========================================================================
// �^�C�g���N���X
class CTitle : public CScene
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	enum SCENETYPE
	{
		SCENETYPE_NONE = 0,			// �Ȃɂ��Ȃ�
		SCENETYPE_FADEIN,			// �t�F�[�h�C��
		SCENETYPE_FADEOUT_LOGO,		// ���S�t�F�[�h�A�E�g
		SCENETYPE_SHOULDTUTORIAL,	// �`���[�g���A���m�F
		SCENETYPE_MAX
	};

	CTitle();
	~CTitle();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	// �V�[���̎��
	void SetSceneType(SCENETYPE type) { m_SceneType = type; }
	SCENETYPE GetSceneType() { return m_SceneType; }

	// ���̑�
	CTitle_PressEnter* GetTitlePressEnter() { return m_pPressEnter; }

	// �ÓI�֐�
	static CTitle* GetInstance();	// �C���X�^���X�擾
	static CTitle* Create();		// ��������

private:

	//=============================
	// �֐��|�C���^
	//=============================
	typedef void(CTitle::* SCENE_FUNC)();
	static SCENE_FUNC m_SceneFunc[];

	//=============================
	// �����o�֐�
	//=============================
	void SceneNone();			// �Ȃɂ��Ȃ�
	void SceneFadeInLogo();		// ���S�t�F�[�h�C��
	void SceneFadeOutLoGo();		// ���S�t�F�[�h�A�E�g

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fSceneTime;			// �V�[���J�E���^�[
	SCENETYPE m_SceneType;		// �V�[���̎��
	CTitleLogo* m_pLogo;		// ���S�̃|�C���^
	CTitle_PressEnter* m_pPressEnter;	// �v���X�G���^�[
	static CTitle* m_pThisPtr;	// ���g�̃|�C���^
};



#endif