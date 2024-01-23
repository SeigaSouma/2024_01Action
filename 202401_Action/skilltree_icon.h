//=============================================================================
// 
// �X�L���c���[�A�C�R������ [skilltree_icon.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_ICON_H_
#define _SKILLTREE_ICON_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"
#include "skilltree_ability.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[�A�C�R����`
class CSkillTree_Icon : public CObject2D
{
public:

	/**
	@brief	�X�L���A�C�R��
	*/
	struct sSkillIcon
	{
		int ID;				// ������ID
		int parentID;		// �e��ID
		int texID;			// �e�N�X�`���C���f�b�N�X
		int needpoint;		// �K�v�|�C���g
		CSkillTree_Ability::eSkillCategory skillCategory;	// �X�L���̃J�e�S���[
		int skillType;		// �X�L���̎��
		int skillStage;	// �X�L���̃X�e�[�W
		MyLib::Vector3 pos;	// �ʒu

		// �f�t�H���g�R���X�g���N�^
		sSkillIcon() : ID(0), parentID(0), texID(0), needpoint(0), pos(), skillCategory(), skillType(0), skillStage(0) {}

		// �p�����[�^���R���X�g���N�^
		sSkillIcon(int nID,int nParentID, int nTexID, int nNeedPoint, const MyLib::Vector3& position, const CSkillTree_Ability::eSkillCategory& category, int type, int stage)
			: ID(nID), parentID(nParentID), texID(nTexID), needpoint(nNeedPoint), pos(position), skillCategory(category), skillType(type), skillStage(stage) {}

		// JSON����̓ǂݍ���
		void from_json(const json& j)
		{
			j.at("ID").get_to(ID);
			j.at("parentID").get_to(parentID);
			j.at("texID").get_to(texID);
			j.at("needpoint").get_to(needpoint);
			j.at("pos").get_to(pos);
			j.at("skillcategory").get_to(skillCategory);
			j.at("skilltype").get_to(skillType);
			j.at("skillstage").get_to(skillStage);
		}

		// JSON�ւ̏�������
		void to_json(json& j) const
		{
			j = json
			{
				{"ID", ID},
				{"parentID", parentID},
				{"texID", texID},
				{"needpoint", needpoint},
				{"pos", pos},
				{"skillcategory", skillCategory},
				{"skilltype", skillType},
				{"skillstage", skillStage},
			};
		}
	};

	CSkillTree_Icon(int nPriority = 8);
	~CSkillTree_Icon();
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetVtx(void) override;

	void SetIconInfo(sSkillIcon iconinfo);	// �A�C�R�����ݒ�
	sSkillIcon GetIconInfo(void);			// �A�C�R�����擾
	static CSkillTree_Icon* Create(void);

private:

	//=============================
	// �񋓌^��`
	//=============================
	// �K����
	enum eMastering
	{
		MASTERING_YET = 0,	// ���K��
		MASTERING_DONE,		// �K���ς�
		MASTERING_POSSIBLE,	// �K���\
		MASTERING_MAX
	};

	// ��ԗ�
	enum eState
	{
		STATE_NONE = 0,	// �Ȃɂ��Ȃ�
		STATE_MAX
	};

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CSkillTree_Icon::*STATE_FUNC)(void);	// ��ԏ����̃��X�g
	static STATE_FUNC m_StateFuncList[];

	//=============================
	// �����o�֐�
	//=============================
	void StateNone(void);	// �����Ȃ����

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fStateTime;			// ��ԑJ�ڃJ�E���^�[
	eState m_state;				// ���
	eMastering m_Mastering;		// �K�����
	sSkillIcon m_SkillIconInfo;	// �X�L���A�C�R�����

};

#endif