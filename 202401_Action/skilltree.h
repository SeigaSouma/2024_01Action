//=============================================================================
// 
//  �X�L���c���[�w�b�_�[ [skilltree.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_H_
#define _SKILLTREE_H_	// ��d�C���N���[�h�h�~

#include "object.h"
#include "skilltree_icon.h"

class CSkillTree_Screen;
class CSkillTree_Cursor;
class CSkillTree_Line;
class CSkillTree_Command;

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[�N���X��`
class CSkillTree : public CObject
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	// ��ԗ�
	enum eState
	{
		STATE_NONE = 0,	// �Ȃɂ��Ȃ�
		STATE_FADEIN,	// �t�F�[�h�C��
		STATE_FADEOUT,	// �t�F�[�h�A�E�g
		STATE_MAX
	};

	CSkillTree(int nPriority = 8);
	~CSkillTree();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void Kill(void);	// �폜
	void LoadJson(void);	// Json����̃��[�h
	void SaveJson(void);	// Json�ւ̃Z�[�u
	void SetScreen(void);	// �X�N���[����ɐݒ�
	void OutScreen(void);	// �X�N���[������J����

	eState GetState() { return m_state; }	// ��Ԏ擾
	void SetMastering(int nIdx, CSkillTree_Icon::eMastering mastering);	// �K���󋵐ݒ�
	std::vector<CSkillTree_Icon*> GetIcon(void) const;	// �A�C�R���擾
	static CSkillTree* GetInstance() { return m_pThisPtr; }	// �C���X�^���X�擾
	static CSkillTree* Create(void);

	// JSON����̓ǂݍ���
	void from_json(const json& j)
	{
		for (const auto& loadData : j.at("iconinfo"))
		{
			CSkillTree_Icon::sSkillIcon iconinfo;
			iconinfo.from_json(loadData);
			m_SkillInfo.push_back(iconinfo);
		}
	}

	// �X�L���A�C�R����JSON�ւ̏�������
	void to_json(json& j) const
	{
		j["iconinfo"] = json::array(); // ��̔z����쐬

		for (const auto& loadData : m_SkillInfo)
		{
			json iconinfo;
			loadData.to_json(iconinfo);
			j["iconinfo"].emplace_back(iconinfo);
		}
	}
private:

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CSkillTree::* STATE_FUNC)(void);	// ��ԏ����̃��X�g
	static STATE_FUNC m_StateFuncList[];

	//=============================
	// �����o�֐�
	//=============================
	void StateNone(void);	// �����Ȃ����
	void StateFadeIn(void);	// �t�F�[�h�C�����
	void StateFadeOut(void);// �t�F�[�h�A�E�g���

	//=============================
	// �����o�ϐ�
	//=============================
	std::vector<CSkillTree_Icon::sSkillIcon> m_SkillInfo;	// �X�L���A�C�R��
	std::vector<CSkillTree_Icon*> m_pSkillIcon;				// �X�L���A�C�R��
	std::vector<CSkillTree_Icon::eMastering> m_SkillIconMastering;	// �X�L���A�C�R���̏K����

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fAlpha;		// �s�����x
	float m_fStateTime;	// ��ԑJ�ڃJ�E���^�[
	eState m_state;		// ���
	CSkillTree_Screen* m_pScreen;	// �X�N���[���̃I�u�W�F�N�g
	CSkillTree_Cursor* m_pCursor;	// �J�[�\���̃I�u�W�F�N�g
	CSkillTree_Command* m_pCommand;	// �R�}���h�̃I�u�W�F�N�g
	bool m_bOnScreen;				// �X�N���[����ɂ��邩�̃t���O
	static CSkillTree* m_pThisPtr;	// ���g�̃|�C���^
};



#endif