//=============================================================================
// 
// �X�L���c���[�E�B���h�E���� [skilltree_window.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_WINDOW_H_
#define _SKILLTREE_WINDOW_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[�E�B���h�E��`
class CSkillTree_Window : public CObject2D
{
public:

	CSkillTree_Window(int nPriority = 8);
	~CSkillTree_Window();

	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;

	static CSkillTree_Window* Create();

};

#endif