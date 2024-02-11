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
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void SetVtx() override;

	static CSkillTree_Cursor* Create(int nIdx);
private:

	//=============================
	// �����o�֐�
	//=============================
	void Controll();
	void CollisionIcon();	// �A�C�R���Ƃ̓����蔻��

	//=============================
	// �����o�ϐ�
	//=============================
	int m_nMyPlayerIdx;		// �v���C���[�C���f�b�N�X�ԍ�
	MyLib::Vector3 m_WorldPos;	// ��΍��W
	MyLib::Vector3 m_DestPos;	// �ڕW���W
	bool m_bHitIcon;			// �A�C�R���̐ڐG�t���O
	bool m_bControllKeyboard;	// �L�[�{�[�h�̑���t���O
};

#endif