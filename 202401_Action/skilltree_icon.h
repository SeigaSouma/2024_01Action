//=============================================================================
// 
// �X�L���c���[�A�C�R������ [skilltree_icon.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_ICON_H_
#define _SKILLTREE_ICON_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

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
		MyLib::Vector3 pos;	// �ʒu
		int nSkillCategory;	
		int nSkillType;
		int nSkillStage;

		// �f�t�H���g�R���X�g���N�^
		sSkillIcon() : ID(0), parentID(0), texID(0), needpoint(0), pos() {}

		// �p�����[�^���R���X�g���N�^
		sSkillIcon(int nID,int nParentID, int nTexID, int nNeedPoint, const MyLib::Vector3& position)
			: ID(nID), parentID(nParentID), texID(nTexID), needpoint(nNeedPoint), pos(position) {}

		// JSON����̓ǂݍ���
		void from_json(const json& j)
		{
			j.at("ID").get_to(ID);
			j.at("parentID").get_to(parentID);
			j.at("texID").get_to(texID);
			j.at("needpoint").get_to(needpoint);
			j.at("pos").get_to(pos);
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
	void SetVtx(void);

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