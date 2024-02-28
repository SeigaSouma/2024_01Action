//=============================================================================
// 
//  ������ʃw�b�_�[ [battleresult_Overall.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _BATTLERESULT_OVERALL_H_
#define _BATTLERESULT_OVERALL_H_	// ��d�C���N���[�h�h�~

#include "object2d.h"
#include "gamerating.h"
#include "multinumber.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ������ʃN���X
class CBattleResult_Overall : public CObject2D
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	// ���
	enum STATE
	{
		STATE_NONE = 0,	// �Ȃɂ��Ȃ�
		STATE_FADEIN,	// �t�F�[�h�C��
		STATE_WAIT_POINT,	// �|�C���g����҂�
		STATE_FADEIN_POINT,	// �|�C���g�t�F�[�h�C��
		STATE_FADEOUT,	// �t�F�[�h�A�E�g
		STATE_MAX
	};

	CBattleResult_Overall(int nPriority = 8);
	~CBattleResult_Overall();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void SetState(STATE state);
	void AllSetting();	// �S�Đݒ�
	void Kill();	// �폜
	bool IsCompleteStaging() { return m_bCompleteStaging; }
	static CBattleResult_Overall* Create(CGameRating::RATING rank, int prevPoint, int currentPoint);	// ��������

private:

	//=============================
	// �񋓌^��`
	//=============================
	// ����
	enum NUMBERTYPE
	{
		NUMBERTYPE_PREV = 0,	// �O��
		NUMBERTYPE_CURRENT,		// ����
		NUMBERTYPE_MAX
	};

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
	typedef void(CBattleResult_Overall::* STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// ��Ԋ֐��̃��X�g

	//=============================
	// �����o�֐�
	//=============================
	// ���
	void StateNone();		// �Ȃɂ��Ȃ�
	void StateFadeIn();		// �t�F�[�h�C��
	void StateRankWait();	// �����N����҂�
	void StateFadeIn_Point();		// �|�C���g�t�F�[�h�C��
	void StateFadeOut();	// �t�F�[�h�A�E�g

	// ����
	void CreatePoint();		// �|�C���g��������

	//=============================
	// �����o�ϐ�
	//=============================
	int m_nPoint[NUMBERTYPE::NUMBERTYPE_MAX];				// �|�C���g
	CMultiNumber* m_pDiffPoint[NUMBERTYPE::NUMBERTYPE_MAX];		// �|�C���g
	STATE m_state;	// ���
	float m_fStateTimer;	// ��ԃ^�C�}�[
	CGameRating::RATING m_OverallRating;	// �����N
	bool m_bCompleteStaging;	// ���o�����t���O
};


#endif