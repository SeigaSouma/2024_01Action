//=============================================================================
// 
//  �T���^�̑܃w�b�_�[ [santabag.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SANTABAG_H_
#define _SANTABAG_H_	// ��d�C���N���[�h�h�~

#include "objectX.h"
#include "listmanager.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �T���^�̑܃N���X��`
class CSantaBag : public CObjectX
{
public:

	CSantaBag(int nPriority = 6);
	~CSantaBag();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	// �����o�֐�
	void Hit();	// �q�b�g����

	// �ÓI�֐�
	static CSantaBag *Create(const MyLib::Vector3 pos);
	static CListManager<CSantaBag> GetListObj() { return m_List; }	// ���X�g�擾

private:

	enum STATE
	{
		STATE_NONE = 0,		// �����Ȃ�
		STATE_DMG,			// �_���[�W
		STATE_INVINCIBLE,	// ���G
		STATE_MAX
	};

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CSantaBag::*STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// ��Ԋ֐��̃��X�g

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	void StateNone();		// �Ȃɂ��Ȃ����
	void StateDamage();		// �_���[�W���
	void StateInvicible();	// ���G���


	//=============================
	// �����o�ϐ�
	//=============================
	float m_fStateTime;	// ��ԃJ�E���^�[
	STATE m_state;		// ���
	D3DXCOLOR m_MatCol;	// �}�e���A���J���[
	int m_nLife;		// ����
	int m_nLifeMax;		// �����̍ő�l
	static CListManager<CSantaBag> m_List;	// ���X�g

};


#endif