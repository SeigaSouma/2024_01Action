//=============================================================================
// 
// �X�L���c���[���������� [skilltree_description.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_DESCRIPTION_H_
#define _SKILLTREE_DESCRIPTION_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[��������`
class CSkillTree_Description : public CObject2D
{
public:

	CSkillTree_Description(int nPriority = 8);
	~CSkillTree_Description();

	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Update(void) override;

	void SetIdxTex(int nIdx);	// �e�N�X�`���̃C���f�b�N�X�ԍ�
	static CSkillTree_Description* Create(void);

private:

	//=============================
	// �����o�֐�
	//=============================
	HRESULT ReadTexture(void);

	//=============================
	// �����o�ϐ�
	//=============================
	int m_nCurrentIdx;	// ���݂̃C���f�b�N�X�ԍ�
	static std::vector<int> m_nTexIdx;	// �e�N�X�`���C���f�b�N�X�ԍ�
	static bool m_bLoadComplete;		// ���[�h�����̃t���O
};

#endif