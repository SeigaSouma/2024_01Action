//=============================================================================
// 
// �X�L���c���[�J�[�\������ [skilltree_cursor.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_CURSOR_H_
#define _SKILLTREE_CURSOR_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[�J�[�\���N���X��`
class CSkillTree_Cursor : public CObject2D
{
public:

	CSkillTree_Cursor(int nPriority = 8);
	~CSkillTree_Cursor();
	
	//  �I�[�o�[���C�h�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetVtx(void) override;

	static CSkillTree_Cursor* Create(int nIdx);
private:

	int m_nMyPlayerIdx;		// �v���C���[�C���f�b�N�X�ԍ�
};

#endif