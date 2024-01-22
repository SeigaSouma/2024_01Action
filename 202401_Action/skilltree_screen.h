//=============================================================================
// 
// �X�L���c���[��ʏ��� [skilltree_screen.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_SCREEN_H_
#define _SKILLTREE_SCREEN_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[��ʒ�`
class CSkillTree_Screen : public CObject2D
{
public:

	CSkillTree_Screen(int nPriority = 8);
	~CSkillTree_Screen();

	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetVtx(void);
	static CSkillTree_Screen* Create(void);

private:

	//=============================
	// �񋓌^��`
	//=============================
	// ��ԗ�
	enum eState
	{
		STATE_NONE = 0,	// �Ȃɂ��Ȃ�
		STATE_FADEIN,	// �t�F�[�h�C��
		STATE_FADEOUT,	// �t�F�[�h�A�E�g
		STATE_MAX
	};

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CSkillTree_Screen::*STATE_FUNC)(void);	// ��ԏ����̃��X�g
	static STATE_FUNC m_StateFuncList[];

	//=============================
	// �����o�֐�
	//=============================
	void StateNone(void);	// �����Ȃ����
	void StateFadeIn(void);	// �t�F�[�h�C�����
	void StateFadeOut(void);// �t�F�[�h�A�E�g���

	// �����o�ϐ�
	float m_fStateTime;	// ��ԑJ�ڃJ�E���^�[
	eState m_state;		// ���

};

#endif