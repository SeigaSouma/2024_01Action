//=============================================================================
// 
// ���X�|�[����e�L�X�g���� [afterrespawn_text.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _AFTERRESPAWN_TEXT_H_
#define _AFTERRESPAWN_TEXT_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ���X�|�[����e�L�X�g�N���X
class CAfterRespawn_Text : public CObject
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	enum TEXTTYPE
	{
		TEXTTYPE_RESET = 0,		// �������Z�b�g
		TEXTTYPE_ADDPOINT,		// ��|�C���g
		TEXTTYPE_MAX
	};

	CAfterRespawn_Text(int nPriority = 0);
	~CAfterRespawn_Text();

	//  �I�[�o�[���C�h�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override {}

	void SetDefaultText();			// �f�t�H���g�e�L�X�g�ݒ�
	void SetText(TEXTTYPE text);	// �e�L�X�g�ݒ�
	void ResetText();	// �e�L�X�g���Z�b�g

	static CAfterRespawn_Text* Create();
	static CAfterRespawn_Text* GetInstance() { return m_pThisPtr; }
private:

	//=============================
	// �񋓌^��`
	//=============================
	enum STATE
	{
		STATE_NONE = 0,
		STATE_APPEARANCE,	// �o��
		STATE_FADEOUT,	// �t�F�[�h�A�E�g
		STATE_MAX
	};

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CAfterRespawn_Text::* STATE_FUNC)();	// ��ԏ����̃��X�g
	static STATE_FUNC m_StateFuncList[];

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	void StateNone() {}	// �ʏ�
	void StateAppearance();	// �o��
	void StateFadeOut();	// �t�F�[�h�A�E�g

	//=============================
	// �����o�ϐ�
	//=============================
	int m_nNumDisp;	// �`�搔
	STATE m_state;			// ���
	float m_fStateTimer;	// ��ԃ^�C�}�[
	CObject2D* m_pText[TEXTTYPE::TEXTTYPE_MAX];	// �e�L�X�g�̃I�u�W�F�N�g
	static CAfterRespawn_Text* m_pThisPtr;		// ���g�̃|�C���^
};
#endif