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
	// �����o�֐�
	//=============================

	//=============================
	// �����o�ϐ�
	//=============================

};

#endif