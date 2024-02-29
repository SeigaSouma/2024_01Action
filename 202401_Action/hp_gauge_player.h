//=============================================================================
// 
//  �v���C���[�̗̑̓Q�[�W�w�b�_�[ [hp_gauge_player.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _HP_GAUGE_PLAYER_H_
#define _HP_GAUGE_PLAYER_H_	// ��d�C���N���[�h�h�~

#include "object2D.h"
#include "object2D_gauge.h"

class CHPGaugeTip;

//==========================================================================
// �N���X��`
//==========================================================================
// �v���C���[�̗̑̓Q�[�W�N���X��`
class CHP_GaugePlayer : public CObject
{
public:

	CHP_GaugePlayer(int nPriority = 11);
	~CHP_GaugePlayer();


	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;


	// �����֐�
	int UpgradeMaxValue(int addvalue);	// �ő�l�̃A�b�v�O���[�h

	void Kill();
	void SetLife(int nLife);
	int GetMaxLife() { return m_nMaxLifeValue; }
	static CHP_GaugePlayer* Create(MyLib::Vector3 pos, int nMaxLife);

private:

	// �񋓌^��`
	enum VTXTYPE
	{
		VTXTYPE_BLACK = 0,	// ���Q�[�W
		VTXTYPE_PINK,		// �s���N�Q�[�W
		VTXTYPE_MAX
	};

	// �\���̒�`
	struct SHP_Gauge
	{
		CObject2D *pObj2D;	// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
		float fMaxWidth;			// ���̍ő�l
		float fWidthDest;			// ���̍���
		float fMaxHeight;			// �����̍ő�l
	};

	// �����o�֐�
	void ChangeColor(int nCntGauge);		// �F�ύX
	void SetVtx(int nIdx);

	// �����o�ϐ�
	int m_nLifeValue;			// �l
	int m_nMaxLifeValue;		// �ő�l
	int m_nOriginLifeValue;	// �����l
	CObject2D_Gauge* m_pObj2DGauge[VTXTYPE_MAX];	// HP�Q�[�W�̃I�u�W�F�N�g
	CHPGaugeTip* m_pTip;	// �Q�[�W�̐�[
};


#endif