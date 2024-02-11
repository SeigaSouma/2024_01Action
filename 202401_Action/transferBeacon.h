//=============================================================================
// 
//  �]�ڃr�[�R���w�b�_�[ [transferBeacon.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TRANSFERBEACON_H_
#define _TRANSFERBEACON_H_	// ��d�C���N���[�h�h�~

#include "objectX.h"
#include "listmanager.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �]�ڃr�[�R���N���X
class CTransferBeacon : public CObjectX
{
public:
	//=============================
	// �񋓌^��`
	//=============================
	enum eTransType	// ���
	{
		TRANSTYPE_ENHANCE = 0,	// �����ɓ]��
		TRANSTYPE_GAMEMAIN,		// �Q�[�����C��
		TRANSTYPE_MAX
	};

	CTransferBeacon(int nPriority = 6);
	~CTransferBeacon();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	// �ÓI�֐�
	static CTransferBeacon *Create(eTransType transtype, const MyLib::Vector3 pos = MyLib::Vector3(0.0f, 150.0f, 0.0f));
	static CListManager<CTransferBeacon> GetListObj() { return m_List; }	// ���X�g�擾

private:

	//=============================
	// �񋓌^��`
	//=============================
	enum STATE	// ���
	{
		STATE_NONE = 0,	// �����Ȃ�
		STATE_TRANSFER,	// �]��
		STATE_MAX
	};

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CTransferBeacon::*STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// ��Ԋ֐��̃��X�g

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	void StateNone();		// �Ȃɂ��Ȃ�
	void StateTransfer();	// �]��

	// ���̑��֐�
	void CollisionPlayer();	// �v���C���[�Ƃ̓����蔻�菈��

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fStateTime;	// ��ԃJ�E���^�[
	STATE m_state;		// ���
	eTransType m_TransType;	// �]�ڎ��
	static CListManager<CTransferBeacon> m_List;	// ���X�g

};


#endif