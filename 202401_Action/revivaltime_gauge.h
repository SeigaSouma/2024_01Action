//=============================================================================
// 
//  �������ԃQ�[�W�w�b�_�[ [revivaltime_gauge.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _REVIVALTIME_GAUGE_H_
#define _REVIVALTIME_GAUGE_H_	// ��d�C���N���[�h�h�~

#include "object2D_gauge.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �������ԃQ�[�W�N���X��`
class CRevivalTimeGauge : public CObject
{
public:

	CRevivalTimeGauge(float fadetime, int nPriority = 8);
	~CRevivalTimeGauge();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void Kill(void);	// �폜


	static CRevivalTimeGauge* Create(const float fadetime, MyLib::Vector3 pos);
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

	// ��ԋ�
	enum eState
	{
		STATE_NONE = 0, // �Ȃɂ��Ȃ�
		STATE_FADEIN,	// �t�F�[�h�C��
		STATE_FADEOUT,	// �t�F�[�h�A�E�g
		STATE_TIMECOMPLETE,	// ���Ԃ��Ȃ��Ȃ����u��
		STATE_MAX
	};

	//=============================
	// �֐��|�C���^
	//=============================
	typedef void(CRevivalTimeGauge::* STATE_FUNC)(void);
	static STATE_FUNC m_StateFunc[];

	//=============================
	// �����o�֐�
	//=============================
	void StateNone(void);		// �Ȃɂ��Ȃ�
	void StateFadeIn(void);		// �t�F�[�h�C��
	void StateFadeOut(void);	// �t�F�[�h�A�E�g
	void StateTimeComplete(void);	// ���Ԃ��Ȃ��Ȃ����u��

	//=============================
	// �����o�ϐ�
	//=============================
	eState m_state;		// ���
	float m_fStateTime;	// ��Ԃ̃J�E���^�[

	float m_fValue;		// �^�C�}�[�̒l
	const float m_fFadeTime;	// �t�F�[�h�ɂ����鎞��
	CObject2D_Gauge* m_pObj2DGauge[VTXTYPE_MAX];	// HP�Q�[�W�̃I�u�W�F�N�g
};


#endif