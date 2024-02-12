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
class CSkillTree_Window;
class CSkillTree_Description;
class CSkillTree_Reset;

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
		STATE_RESET,	// ���Z�b�g
		STATE_MAX
	};

	CSkillTree(int nPriority = 8);
	~CSkillTree();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();	// �폜
	void LoadJson();	// Json����̃��[�h
	void SaveJson();	// Json�ւ̃Z�[�u
	void SetScreen();	// �X�N���[����ɐݒ�
	void OutScreen();	// �X�N���[������J����
	void OutReset();	// ���Z�b�g�J����

	eState GetState() { return m_state; }	// ��Ԏ擾
	void SetMastering(int nIdx, CSkillTree_Icon::eMastering mastering);	// �K���󋵐ݒ�
	void SetMastering(std::vector<CSkillTree_Icon::eMastering> mastering);	// �K���󋵐ݒ�
	std::vector<CSkillTree_Icon::eMastering> GetMastering();			// �K���󋵎擾
	void ResetMastering();												// �K���󋵃��Z�b�g
	std::vector<CSkillTree_Icon*> GetIcon() const;	// �A�C�R���擾
	std::vector<CSkillTree_Icon::sSkillIcon> GetIconInfo() { return m_SkillInfo; }
	CSkillTree_Description* GetDescription() { return m_pDescription; }	// �������擾

	static CSkillTree* GetInstance() { return m_pThisPtr; }	// �C���X�^���X�擾
	static CSkillTree* Create();

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
	typedef void(CSkillTree::* STATE_FUNC)();	// ��ԏ����̃��X�g
	static STATE_FUNC m_StateFuncList[];

	//=============================
	// �����o�֐�
	//=============================
	void StateNone();	// �����Ȃ����
	void StateFadeIn();	// �t�F�[�h�C�����
	void StateFadeOut();// �t�F�[�h�A�E�g���
	void StateReset();		// ���Z�b�g���

	//=============================
	// �����o�ϐ�
	//=============================
	std::vector<CSkillTree_Icon::sSkillIcon> m_SkillInfo;	// �X�L���A�C�R��
	std::vector<CSkillTree_Icon*> m_pSkillIcon;				// �X�L���A�C�R��

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fAlpha;		// �s�����x
	float m_fStateTime;	// ��ԑJ�ڃJ�E���^�[
	eState m_state;		// ���
	CSkillTree_Screen* m_pScreen;	// �X�N���[���̃I�u�W�F�N�g
	CSkillTree_Cursor* m_pCursor;	// �J�[�\���̃I�u�W�F�N�g
	CSkillTree_Command* m_pCommand;	// �R�}���h�̃I�u�W�F�N�g
	CSkillTree_Window* m_pWindow;	// �E�B���h�E�̃I�u�W�F�N�g
	CSkillTree_Description* m_pDescription;	// �������̃I�u�W�F�N�g
	CSkillTree_Reset* m_pReset;				// ���Z�b�g�p�̃I�u�W�F�N�g
	bool m_bOnScreen;				// �X�N���[����ɂ��邩�̃t���O
	static CSkillTree* m_pThisPtr;	// ���g�̃|�C���^
};



#endif