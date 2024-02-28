//=============================================================================
// 
//  �v���C���[�̃X�^�~�i�Q�[�W�w�b�_�[ [stamina_gauge_player.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _STAMINA_GAUGE_PLAYER_H_
#define _STAMINA_GAUGE_PLAYER_H_	// ��d�C���N���[�h�h�~

#include "object2D_gauge.h"

class CHPGaugeTip;

//==========================================================================
// �N���X��`
//==========================================================================
// �v���C���[�̃X�^�~�i�Q�[�W�N���X
class CStaminaGauge_Player : public CObject
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	// ���
	enum STATE
	{
		STATE_NORMAL = 0,	// �ʏ�
		STATE_SUB,			// ���Z
		STATE_QUICKHEAL,	// �}����
		STATE_MAX
	};

	CStaminaGauge_Player(int nPriority = 7);
	~CStaminaGauge_Player();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();	// �폜

	// �����֐�
	void UpgradeMaxValue(int addvalue);		// �ő�l�̃A�b�v�O���[�h
	void UpgradeAutoHeal(float multiply);	// �����񕜂̃A�b�v�O���[�h

	// �l�̊֐�
	void AddValue(float value);	// �l���Z
	void SubValue(float value);	// �l���Z
	void SetValue(float value);	// �l�ݒ�
	float GetValue();			// �l�擾
	float GetMaxValue() { return m_fMaxStaminaValue; }	// �ő�l�擾
	float GetAutoHealValue() { return m_fAutoHeal; }	// ���R�񕜗ʎ擾

	void SetState(STATE state);
	STATE GetState() { return m_state; }

	static CStaminaGauge_Player* Create(MyLib::Vector3 pos, int maxvalue);
private:

	//=============================
	// �񋓌^��`
	//=============================
	// ���_��
	enum VTXTYPE
	{
		VTXTYPE_BLACK = 0,	// ���Q�[�W
		VTXTYPE_GAUGE,		// �Q�[�W����
		VTXTYPE_MAX
	};

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CStaminaGauge_Player::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԍn
	void StateNormal();	// �ʏ���
	void StateSub();	// ���Z���
	void StateQuickHealing();	// �}���񕜏��

	// ���̑�
	void ChangeColor();		// �F�ύX
	void CorrectionValue();	// �l�̕␳
	void SetVtx(int nIdx);

	//=============================
	// �����o�ϐ�
	//=============================
	STATE m_state;					// ���
	float m_fStateTime;				// ��Ԃ̃J�E���^�[
	float m_fStaminaValue;			// �X�^�~�i�̒l
	float m_fMaxStaminaValue;		// �X�^�~�i�̍ő�l
	float m_fOriginStaminaValue;	// �X�^�~�i�̏����l
	float m_fAutoHeal;				// ������
	CObject2D_Gauge* m_pObj2DGauge[VTXTYPE_MAX];	// HP�Q�[�W�̃I�u�W�F�N�g
	CHPGaugeTip* m_pTip;	// �Q�[�W�̐�[
};


#endif