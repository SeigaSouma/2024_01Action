//=============================================================================
// 
// �X�L���c���[���Z�b�g���� [skilltree_reset.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_RESET_H_
#define _SKILLTREE_RESET_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[���Z�b�g��`
class CSkillTree_Reset : public CObject2D
{
public:

	CSkillTree_Reset(int nPriority = 8);
	~CSkillTree_Reset();
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();
	static CSkillTree_Reset* Create();

private:

	//=============================
	// �����o�֐�
	//=============================
	enum SELECT
	{
		SELECT_CANCEL = 0,	// �L�����Z��
		SELECT_OK,			// OK
		SELECT_MAX
	};

	//=============================
	// �����o�֐�
	//=============================
	void CreateSelect();	// �I��������
	void UpdateSelect();	// �I�����X�V

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fFlashTime;	// �_�Ŏ���
	int m_nSelect;	// �I����
	
	CObject2D* m_pSelectObj[SELECT_MAX];	// �I�����̃I�u�W�F�N�g
	static std::vector<int> m_nTexIdx;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
};

#endif