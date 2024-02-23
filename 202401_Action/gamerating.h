//=============================================================================
// 
// �Q�[���]������ [gamerating.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _GAMERATING_H_
#define _GAMERATING_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

//==========================================================================
// �X�L���c���[�\�͂�Strategy
//==========================================================================
class CGameRating
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	enum RATING
	{
		RATING_B = 0,	// B�]��
		RATING_A,		// A�]��
		RATING_S,		// S�]��
		RATING_MAX
	};

	CGameRating();
	~CGameRating();

	HRESULT Init();
	void Uninit();
	void Update();

	void SetReceiveDamage(int damage) { m_nReceiveDamage = damage; }	// ��_���[�W�ݒ�
	int GetReceiveDamage() { return m_nReceiveDamage; }					// ��_���[�W�擾
	void AddReceiveDamage(int damage) { m_nReceiveDamage += damage; }	// ��_���[�W���Z
	void SetNumDead(int num) { m_nNumDead = num; }						// ���S�񐔐ݒ�
	int GetNumDead() { return m_nNumDead; }								// ���S�񐔎擾
	void AddNumDead() { m_nNumDead++; }									// ���S�񐔉��Z
	void SetClearTime(float time) { m_fClearTime = time; }				// �N���A���Ԑݒ�
	float GetClearTime() { return m_fClearTime; }						// �N���A���Ԏ擾
	void AddClearTime(float time) { m_fClearTime += time; }				// �N���A���ԉ��Z
	RATING GetRating() { return m_rating; }								// �]���擾

	static CGameRating* Create();		// ��������

private:

	//=============================
	// �����o�֐�
	//=============================

	//=============================
	// �����o�ϐ�
	//=============================
	int m_nReceiveDamage;	// ��_���[�W
	int m_nNumDead;			// ���S��
	float m_fClearTime;		// �N���A����
	RATING m_rating;		// �]��

};

#endif