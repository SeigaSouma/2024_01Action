//=============================================================================
// 
// �`���[�g���A�������邩�m�F���� [tutorial_checkshould.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TUTORIAL_CHECKSHOULD_H_
#define _TUTORIAL_CHECKSHOULD_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �`���[�g���A���m�F�N���X
class CTutorialCheckShould : public CObject2D
{
public:

	CTutorialCheckShould(int nPriority = 8);
	~CTutorialCheckShould();
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();
	static CTutorialCheckShould* Create();

private:

	//=============================
	// �����o�֐�
	//=============================
	enum SELECT
	{
		SELECT_NO = 0,	// ���̂܂܃Q�[��
		SELECT_OK,		// ���K�X�e�[�W
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
	int m_nSelect;		// �I����
	
	CObject2D* m_pSelectObj[SELECT_MAX];	// �I�����̃I�u�W�F�N�g
	static std::vector<int> m_nTexIdx;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
};

#endif