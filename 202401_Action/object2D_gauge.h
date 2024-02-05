//=============================================================================
// 
//  �I�u�W�F�N�g2D�Q�[�W�w�b�_�[ [object2D_gauge.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _OBJECT2D_GAUGE_H_
#define _OBJECT2D_GAUGE_H_	// ��d�C���N���[�h�h�~

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �I�u�W�F�N�g2D�Q�[�W�N���X��`
class CObject2D_Gauge : public CObject2D
{
public:

	CObject2D_Gauge(int nPriority = 6);
	virtual ~CObject2D_Gauge();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init(void) override;
	virtual HRESULT Init(float width, float height, int maxvalue, std::string texturepath = "");
	virtual void Update(void) override;
	virtual void SetVtx(void) override;


	// �����֐�
	MyLib::Vector3 UpgradeMaxValue(int addvalue);	// �ő�l�̃A�b�v�O���[�h

	// ���̑��֐�
	void SetValue(int value);				// �l�ݒ�
	int GetValue(void) { return m_nValue; }	// �l�擾
	void SetMaxValue(int value) { m_nMaxValue = value; }	// �l�̍ő�l�ݒ�
	int GetMaxValue(void) { return m_nMaxValue; }			// �l�̍ő�l�擾
	void SetMaxWidth(float width) { m_fMaxWidth = width; }	// ���̍ő�l�ݒ�
	float GetMaxWidth(void) { return m_fMaxWidth; }			// ���̍ő�l�擾
	void SetMoveFactor(float factor) { m_fMoveFactor = factor; }	// �ړ��̌W���ݒ�

	// �ÓI�֐�
	static CObject2D_Gauge *Create(float width, float height, int maxvalue, std::string texturepath = "", int nPriority = 6);	// ��������
	CObject2D_Gauge *GetObject2Dgauge(void) { return this; }
protected:

private:
	
	//=============================
	// �����o�֐�
	//=============================
	void GaugeDecrement(void);

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fOriginWidth;	// ���̍ő�l
	float m_fMaxWidth;		// ���̍ő�l
	float m_fWidthDest;		// �ڕW�̕�
	float m_fMaxHeight;		// �����̍ő�l
	float m_fMoveFactor;	// �ړ��̌W��
	int m_nValue;			// ���݂̒l
	int m_nOriginValue;		// �����l
	int m_nMaxValue;		// �ő�l
};



#endif