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
	// ��ʃ����N
	enum RANK
	{
		RANK_B = 0,	// B�����N
		RANK_A,		// A�����N
		RANK_S,		// S�����N
		RANK_MAX
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
	// �����o�֐�
	//=============================
	void CreateRank();		// �N���A�����N����
	void CreateClearTime();	// �N���A�^�C������
	void CreateDamage();	// ��_���[�W����
	void CreateDead();		// ���S�񐔐���

	//=============================
	// �����o�ϐ�
	//=============================
	CObject2D* m_pRank[CGameRating::RATINGTYPE::RATINGTYPE_MAX];		// ��ނ��Ƃ̃����N
	CMultiNumber* m_pNumber[CGameRating::RATINGTYPE::RATINGTYPE_MAX];	// ��ނ��Ƃ̐���
	CMultiNumber* m_pClearTime[3];	// ��ނ��Ƃ̐���
	CMultiNumber* m_pDamage;	// ��ނ��Ƃ̐���
	CMultiNumber* m_pDead;	// ��ނ��Ƃ̐���
};


#endif