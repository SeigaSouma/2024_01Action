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

	enum RATINGTYPE
	{
		RATINGTYPE_TIME = 0,	// �N���A�^�C��
		RATINGTYPE_DMG,			// ��_���[�W
		RATINGTYPE_DEAD,		// ���S��
		RATINGTYPE_MAX
	};

	//=============================
	// �\���̒�`
	//=============================
	struct sRating
	{
		float clearTime;	// �N���A����
		int receiveDamage;	// ��_���[�W
		int numDead;		// ���S��
		RATING rating;		// �]��

		sRating() : clearTime(0.0f), receiveDamage(0), numDead(0), rating() {}
		sRating(float time, int damage, int dead) : clearTime(time), receiveDamage(damage), numDead(dead), rating() {}
	};

	CGameRating();
	~CGameRating();

	HRESULT Init();
	void Uninit();
	void Update();

	void SetReceiveDamage(int damage) { m_RatingInfo.receiveDamage = damage; }	// ��_���[�W�ݒ�
	int GetReceiveDamage() { return m_RatingInfo.receiveDamage; }				// ��_���[�W�擾
	void AddReceiveDamage(int damage) { m_RatingInfo.receiveDamage += damage; }	// ��_���[�W���Z
	void SetNumDead(int num) { m_RatingInfo.numDead = num; }					// ���S�񐔐ݒ�
	int GetNumDead() { return m_RatingInfo.numDead; }							// ���S�񐔎擾
	void AddNumDead() { m_RatingInfo.numDead++; }								// ���S�񐔉��Z
	void SetClearTime(float time) { m_RatingInfo.clearTime = time; }			// �N���A���Ԑݒ�
	float GetClearTime() { return m_RatingInfo.clearTime; }						// �N���A���Ԏ擾
	void AddClearTime(float time) { m_RatingInfo.clearTime += time; }			// �N���A���ԉ��Z
	RATING GetRating() { return m_RatingInfo.rating; }							// �]���擾
	sRating GetRatingInfo() { return m_RatingInfo; }							// �]�����擾

	RATING CalculateRank(const sRating& result, const std::map<std::string, sRating>& rankStandards);
	RATING CalculateClearTimeRank(const float time);		// ���Ԃ̃����N����o��
	RATING CalculateRecieveDamageRank(const int damage);	// ��_���[�W�̃����N����o��
	RATING CalculateNumDeadRank(const int dead);			// ���S�񐔂̃����N����o��


	static CGameRating* Create();		// ��������
	static void ReadText();

private:

	//=============================
	// �����o�֐�
	//=============================

	//=============================
	// �����o�ϐ�
	//=============================
	sRating m_RatingInfo;	// �]�����
	static std::map<std::string, std::map<std::string, sRating>> m_RatingStandards;	// �]���
};

#endif