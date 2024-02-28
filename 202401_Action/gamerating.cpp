//=============================================================================
// 
// �Q�[���]������ [gamerating.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "gamerating.h"
#include "game.h"
#include "gamemanager.h"
#include "manager.h"
#include "calculation.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const std::string TEXT_STANDARDS = "data\\TEXT\\gamerating\\ratingStandards.txt";		// �]����̊O���t�@�C����
	const std::map<std::string, int> PRIORITY_RANK =
	{
		{"RankS", 3}, // S�����N�̗D�揇��
		{"RankA", 2}, // A�����N�̗D�揇��
		{"RankB", 1}  // B�����N�̗D�揇��
	};
	const int POINT_RANK[] =
	{
		5, 6, 7
	};
	const int CONDITION_S = 7;	// S�����N����
	const int CONDITION_A = 5;	// A�����N����
}

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
std::map<std::string, std::map<std::string, CGameRating::sRating>> CGameRating::m_RatingStandards = {};	// �]���

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CGameRating::CGameRating()
{
	// �l�̃N���A
	m_RatingInfo = sRating();	// �]�����
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CGameRating::~CGameRating()
{

}

//==========================================================================
// ��������
//==========================================================================
CGameRating* CGameRating::Create()
{
	// �������m��
	CGameRating* pRating = DEBUG_NEW CGameRating;

	if (pRating != nullptr)
	{
		// ����������
		if (FAILED(pRating->Init()))
		{
			return nullptr;
		}
	}

	return pRating;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CGameRating::Init()
{
	

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CGameRating::Uninit()
{
	delete this;
}

//==========================================================================
// �X�V����
//==========================================================================
void CGameRating::Update()
{
	
}

//==========================================================================
// �e�L�X�g�ǂݍ��ݏ���
//==========================================================================
void CGameRating::ReadText()
{
	// �t�@�C�����J��
	std::ifstream File(TEXT_STANDARDS);
	if (!File.is_open())
	{
		return;
	}

	// ���Z�b�g
	m_RatingStandards.clear();

	// �f�[�^�ǂݍ���
	std::string line;
	while (std::getline(File, line))
	{
		// �R�����g�̓X�L�b�v
		if (line.empty() || 
			line[0] == '#')
		{
			continue;
		}

		// �X�g���[���쐬
		std::istringstream lineStream(line);

		// �X�e�[�W�ƃ����N
		std::string stage, rank;

		// �]���
		float time;
		int damage;
		int dead;

		// �]����ǉ�
		if (lineStream >> stage >> rank >> time >> damage >> dead)
		{
			m_RatingStandards[stage][rank] = sRating(time, damage, dead);

			// �����N���蓖��
			if (rank == "RankB") {
				m_RatingStandards[stage][rank].rating = RATING::RATING_B;
			}
			else if (rank == "RankA") {
				m_RatingStandards[stage][rank].rating = RATING::RATING_A;
			}
			else if (rank == "RankS") {
				m_RatingStandards[stage][rank].rating = RATING::RATING_S;
			}
		}
	}

	// �t�@�C�������
	File.close();
}

//==========================================================================
// ���������N�|�C���g����o��
//==========================================================================
int CGameRating::CalculateOverrallRankPoint(RATING allRank)
{
	return POINT_RANK[allRank];
}

//==========================================================================
// �X�e�[�W���̃����N����o��
//==========================================================================
CGameRating::RATING CGameRating::CalculateRank(RATING timeRank, RATING dmgRank, RATING deadRank)
{
	int allpoint = static_cast<int>(timeRank + dmgRank + deadRank + RATINGTYPE_MAX);

	if (allpoint >= CONDITION_S)
	{
		return RATING::RATING_S;
	}
	if (allpoint >= CONDITION_A)
	{
		return RATING::RATING_A;
	}

	return RATING::RATING_B;
}

//==========================================================================
// �N���A�^�C���̃����N����o��
//==========================================================================
CGameRating::RATING CGameRating::CalculateClearTimeRank(const float time)
{

	std::string stageName = "Stage" + std::to_string(CGame::GetInstance()->GetGameManager()->GetNowStage());

	// �]���̍ō��l�ێ�
	RATING maxRating = RATING::RATING_B;
	int maxPriority = 0;

	for (const auto& rankInfo : m_RatingStandards[stageName]) 
	{
		const sRating& standard = rankInfo.second;

		// �D�揇�ʊ���o��
		int priority = PRIORITY_RANK.at(rankInfo.first);

		// ��`�F�b�N
		if (time <= standard.clearTime && 
			priority >= maxPriority)
		{
			maxPriority = priority;
			maxRating = rankInfo.second.rating;
		}
	}

	return maxRating;
}

//==========================================================================
// ��_���[�W�̃����N����o��
//==========================================================================
CGameRating::RATING CGameRating::CalculateRecieveDamageRank(const int damage)
{
	std::string stageName = "Stage" + std::to_string(CGame::GetInstance()->GetGameManager()->GetNowStage());

	// �]���̍ō��l�ێ�
	RATING maxRating = RATING::RATING_B;
	int maxPriority = 0;

	for (const auto& rankInfo : m_RatingStandards[stageName])
	{
		const sRating& standard = rankInfo.second;

		// �D�揇�ʊ���o��
		int priority = PRIORITY_RANK.at(rankInfo.first);

		// ��`�F�b�N
		if (damage <= standard.receiveDamage &&
			priority >= maxPriority)
		{
			maxPriority = priority;
			maxRating = rankInfo.second.rating;
		}
	}

	return maxRating;
}

//==========================================================================
// ���S�񐔂̃����N����o��
//==========================================================================
CGameRating::RATING CGameRating::CalculateNumDeadRank(const int dead)
{
	std::string stageName = "Stage" + std::to_string(CGame::GetInstance()->GetGameManager()->GetNowStage());

	// �]���̍ō��l�ێ�
	RATING maxRating = RATING::RATING_B;
	int maxPriority = 0;

	for (const auto& rankInfo : m_RatingStandards[stageName])
	{
		const sRating& standard = rankInfo.second;

		// �D�揇�ʊ���o��
		int priority = PRIORITY_RANK.at(rankInfo.first);

		// ��`�F�b�N
		if (dead <= standard.numDead &&
			priority >= maxPriority)
		{
			maxPriority = priority;
			maxRating = rankInfo.second.rating;
		}
	}

	return maxRating;
}