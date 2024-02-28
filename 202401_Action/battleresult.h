//=============================================================================
// 
//  ��ʃw�b�_�[ [battleresult.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _BATTLERESULT_H_
#define _BATTLERESULT_H_	// ��d�C���N���[�h�h�~

#include "object2d.h"
#include "gamerating.h"
#include "multinumber.h"

class CBattleResult_Overall;

//==========================================================================
// �N���X��`
//==========================================================================
// ��ʃN���X
class CBattleResult : public CObject2D
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	// ���
	enum STATE
	{
		STATE_FADEIN = 0,	// �t�F�[�h�C��
		STATE_RANKWAIT,		// �����N����҂�
		STATE_RANKIN,		// �����N����
		STATE_RANKIN_OVERALL,		// ���������N����
		STATE_RETURNWAIT,	// �����ҋ@
		STATE_FADEOUT,		// �t�F�[�h�A�E�g
		STATE_MAX
	};

	CBattleResult(int nPriority = 8);
	~CBattleResult();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();	// �폜
	static CBattleResult* Create();	// ��������

private:

	//=============================
	// �\���̒�`
	//=============================
	struct sResultRank
	{
		CObject2D* pObj2D;			// �I�u�W�F�N�g2D
		CGameRating::RATING rating;	// �����N
	};

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CBattleResult::* STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// ��Ԋ֐��̃��X�g

	//=============================
	// �����o�֐�
	//=============================
	// ���
	void StateFadeIn();		// �t�F�[�h�C��
	void StateRankWait();	// �����N����҂�
	void StateRankIn();		// �����N����
	void StateRankIn_OverAll();		// ���������N����
	void StateReturnWait();	// �����ҋ@
	void StateFadeOut();	// �t�F�[�h�A�E�g

	// ����
	void CreateRank();		// �N���A�����N����
	void CreateClearTime();	// �N���A�^�C������
	void CreateDamage();	// ��_���[�W����
	void CreateDead();		// ���S�񐔐���
	void CreateRankOverall(int prevPoint);	// ���������N����

	// ���̑�
	void AllSetting();	// �S�Đݒ�

	//=============================
	// �����o�ϐ�
	//=============================
	sResultRank m_RankInfo[CGameRating::RATINGTYPE::RATINGTYPE_MAX];	// ��ނ��Ƃ̃����N
	CObject2D* m_pRank[CGameRating::RATINGTYPE::RATINGTYPE_MAX];		// ��ނ��Ƃ̃����N
	CMultiNumber* m_pClearTime[3];		// ��ނ��Ƃ̐���
	CMultiNumber* m_pDamage;			// ��ނ��Ƃ̐���
	CMultiNumber* m_pDead;				// ��ނ��Ƃ̐���
	CBattleResult_Overall* m_pOverall;	// ����
	STATE m_state;	// ���
	float m_fStateTimer;	// ��ԃ^�C�}�[
	float m_fSurvivalTimer;	// �����^�C�}�[
	bool m_bCompleteStaging;	// ���o�����t���O
};


#endif