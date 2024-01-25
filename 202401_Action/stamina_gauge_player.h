//=============================================================================
// 
//  �v���C���[�̃X�^�~�i�Q�[�W�w�b�_�[ [stamina_gauge_player.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _STAMINA_GAUGE_PLAYER_H_
#define _STAMINA_GAUGE_PLAYER_H_	// ��d�C���N���[�h�h�~

#include "object2D_gauge.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �v���C���[�̃X�^�~�i�Q�[�W�N���X��`
class CStaminaGauge_Player : public CObject
{
public:

	CStaminaGauge_Player(int nPriority = 7);
	~CStaminaGauge_Player();

	static CStaminaGauge_Player *Create(MyLib::Vector3 pos, int maxvalue);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void Kill(void);	// �폜

	// �����֐�
	void UpgradeMaxValue(int addvalue);	// �ő�l�̃A�b�v�O���[�h

	// �l�̊֐�
	void AddValue(float value);	// �l���Z
	void SubValue(float value);	// �l���Z
	void SetValue(float value);	// �l�ݒ�
	float GetValue(void);		// �l�擾

private:

	//=============================
	// �񋓌^��`
	//=============================
	// ���_��
	enum VTXTYPE
	{
		VTXTYPE_BLACK = 0,	// ���Q�[�W
		VTXTYPE_GAUGE,		// �Q�[�W����
		VTXTYPE_FRAM,		// �g
		VTXTYPE_MAX
	};

	// ���_��
	enum STATE
	{
		STATE_NORMAL = 0,	// �ʏ�
		STATE_SUB,			// ���Z
		STATE_MAX
	};

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CStaminaGauge_Player::* STATE_FUNC)(void);
	static STATE_FUNC m_StateFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԍn
	void StateNormal(void);	// �ʏ���
	void StateSub(void);	// ���Z���

	// ���̑�
	void ChangeColor(void);		// �F�ύX
	void CorrectionValue(void);	// �l�̕␳

	//=============================
	// �����o�ϐ�
	//=============================
	STATE m_state;				// ���
	float m_fStateTime;			// ��Ԃ̃J�E���^�[
	float m_fStaminaValue;	// �X�^�~�i�̒l
	float m_fMaxStaminaValue;	// �X�^�~�i�̍ő�l
	float m_fOriginStaminaValue;	// �X�^�~�i�̏����l
	float m_fAutoHeal;		// ������
	CObject2D_Gauge* m_pObj2DGauge[VTXTYPE_MAX];	// HP�Q�[�W�̃I�u�W�F�N�g
};


#endif