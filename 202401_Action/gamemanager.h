//=============================================================================
// 
//  �Q�[���}�l�[�W���w�b�_�[ [gamemanager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_	// ��d�C���N���[�h�h�~

#include "constans.h"
#include "skilltree_icon.h"

class CSkillTree_Ability;
class CGameRating;

//==========================================================================
// �N���X��`
//==========================================================================
// �Q�[���}�l�[�W���N���X��`
class CGameManager
{
public:

	// �V�[���̎��
	enum SceneType
	{
		SCENE_MAIN = 0,			// ���C��
		SCENE_MAINRESULT,		// ���C�����U���g
		SCENE_DURING_MAINRESULT,		// ���C�����U���g��
		SCENE_MAINCLEAR,		// ���C���N���A
		SCENE_BEFOREBATTLE,		// �퓬����
		SCENE_BATTLESTART,		// �퓬�J�n
		SCENE_ENHANCE,			// ����
		SCENE_SKILLTREE,		// �X�L���c���[
		SCENE_BOSS,				// �{�X
		SCENE_TRANSITIONWAIT,	// �J�ڑҋ@
		SCENE_TRANSITION,		// �]��
		SCENE_REASPAWN,			// ����
		SCENE_RESULT,			// ���U���g
		SCENE_MAX
	};

	enum AttackType
	{
		ATTACK_NORMAL = 0,	// �ʏ�U��
		ATTACK_STRONG,		// ���U��
		ATTACK_COUNTER,		// �����U��
		ATTACK_MAX
	};

	CGameManager();
	~CGameManager();

	HRESULT Init();
	void Uninit();
	void Update();

	void SetType(SceneType type);	// �V�[���̎�ސݒ�
	SceneType GetType();		// �V�[���̎�ގ擾
	void SetEnableRush() { m_bEndRush = true; }		// ���b�V���̏I���t���O��L���ɂ���
	bool IsRushEnd() { return m_bEndRush; }			// �I���̃t���O�擾
	void SetEnableControll(bool bControll) { m_bControll = bControll; }		// ����t���O��؂�ւ�����
	bool IsControll() { return m_bControll; }				// ����̃t���O�擾
	bool IsEndNormalStage() { return m_bEndNormalStage; }	// �ʏ�X�e�[�W�̃A�I������
	void AddNowStage();	// �X�e�[�W�̉��Z
	int GetNowStage();	// ���݂̃X�e�[�W�擾
	void SetNumStage(int nStage);				// �X�e�[�W�̑����ݒ�
	int GetNumStage() { return m_nNumStage; }	// �X�e�[�W�̑����擾

	void SetPrevEnhance();	// �O��̋������e�ݒ�

	CGameRating* GetGameRating();	// �Q�[���]���擾

	void GameClearSettings();	// �Q�[���N���A���̐ݒ�
	void GameResultSettings();	// �Q�[�����U���g�̐ݒ�
	static CGameManager *Create();	// ��������

private:

	// �����o�֐�
	void SceneTransition();	// �J�ڒ�
	void SceneEnhance();
	void SceneReaspawn();	// ����
	void SetEnemy();
	void SetBoss();

	void UpdateGalleryVolume();	// �ϋq���ʍX�V

	// �����o�ϐ�
	CSkillTree_Ability* m_pSkilltreeAbillity;	// �X�L���c���[�\�͂̃|�C���^
	SceneType m_SceneType;	// �V�[���̎��
	bool m_bEndRush;		// ���b�V�����I��������
	bool m_bControll;		// ����ł��邩
	bool m_bEndNormalStage;	// �ʏ�X�e�[�W���I��������
	bool m_bGameStart;		// �Q�[���J�n���̃t���O
	int m_nNowStage;		// ���݂̃X�e�[�W
	int m_nNumStage;		// �X�e�[�W�̑���
	int m_nPrevPoint;		// �O��̃|�C���g
	std::vector<CSkillTree_Icon::eMastering> m_PrevSkillIconMastering;	// �O��̃X�L���A�C�R���̏K����
	std::vector<CSkillTree_Icon*> m_p_PrevSkillIcon;	// �O��̃X�L���A�C�R��
	std::vector<CGameRating*> m_pGameRating;			// �Q�[���]��
};



#endif