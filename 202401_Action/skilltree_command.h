//=============================================================================
// 
// �X�L���c���[�R�}���h���� [skilltree_command.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_COMMAND_H_
#define _SKILLTREE_COMMAND_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[�R�}���h�N���X��`
class CSkillTree_Command : public CObject
{
public:

	CSkillTree_Command(int nPriority = 8, const LAYER layer = LAYER::LAYER_2D);
	~CSkillTree_Command();
	
	//  �I�[�o�[���C�h�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override {}
	void Draw() override {}

	void SetAlpha(float alpha);	// �s�����x�ݒ�
	static CSkillTree_Command* Create();
private:

	//=============================
	// �񋓌^��`
	//=============================
	enum ICONTYPE
	{
		ICONTYPE_BACK = 0,	// ���ǂ�
		ICONTYPE_GET,	// �K��
		ICONTYPE_RESET,	// ���Z�b�g
		ICONTYPE_MAX
	};

	//=============================
	// �����o�ϐ�
	//=============================
	CObject2D* m_pIcon[ICONTYPE_MAX];	// �A�C�R���̃I�u�W�F�N�g
};

#endif