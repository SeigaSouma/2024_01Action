//=============================================================================
// 
// �X�L���c���[���C������ [skilltree_line.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_LINE_H_
#define _SKILLTREE_LINE_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

class CSkillTree_Icon;

#include "object2D.h"
#include "skilltree_ability.h"
#include "listmanager.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[���C����`
class CSkillTree_Line : public CObject2D
{
public:

	/**
	@brief	�X�L�����C��
	*/
	struct sSkillLine
	{
		
		MyLib::Vector3 mypos;		// �����̈ʒu
		MyLib::Vector3 parentpos;	// �e�̈ʒu

		// �f�t�H���g�R���X�g���N�^
		sSkillLine() : mypos(), parentpos() {}

		// �p�����[�^���R���X�g���N�^
		sSkillLine(const MyLib::Vector3& myposition, const MyLib::Vector3& parentposition) : mypos(myposition), parentpos(parentposition) {}
	};

	CSkillTree_Line(int nPriority = 7);
	~CSkillTree_Line();
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void SetVtx() override;

	void SetSkillLineInfo(const MyLib::Vector3& myposition, const MyLib::Vector3& parentposition);
	static CListManager<CSkillTree_Line> GetListObj() { return m_List; }	// ���X�g�擾
	static CSkillTree_Line* Create(const MyLib::Vector3& myposition, const MyLib::Vector3& parentposition);
	static CSkillTree_Line* Create(CSkillTree_Icon* myIcon, CSkillTree_Icon* parentIcon);
private:

	sSkillLine m_SkillLine;	// �X�L�����C��
	CSkillTree_Icon* m_pMyIcon;	// �����̃A�C�R��
	CSkillTree_Icon* m_pParentIcon;	// �e�̃A�C�R��
	static CListManager<CSkillTree_Line> m_List;	// ���X�g
};

#endif