//=============================================================================
// 
// �X�L���c���[���Z�b�g�s�\���� [skilltree_reset_impossible.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_RESET_IMPOSSIBLE_H_
#define _SKILLTREE_RESET_IMPOSSIBLE_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[���Z�b�g�s�\��`
class CSkillTree_ResetImpossible : public CObject2D
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	enum ERRORTYPE
	{
		ERRORTYPE_POINT = 0,	// �|�C���g�s��
		ERRORTYPE_NOGET,		// ���K��
		SELECT_MAX
	};

	CSkillTree_ResetImpossible(int nPriority = 9);
	~CSkillTree_ResetImpossible();
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	static CSkillTree_ResetImpossible* Create(ERRORTYPE error);

private:

	//=============================
	// �����o�֐�
	//=============================

	//=============================
	// �����o�ϐ�
	//=============================
	ERRORTYPE m_ErrorType;	// �G���[�̎��
	float m_fAlphaTime;	// �s�����x�^�C�}�[
	static CSkillTree_ResetImpossible* m_pThisPtr;	// ���g�̃|�C���^
};

#endif