//=============================================================================
// 
//  �퓬�������� [battleresult.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "battleresult.h"
#include "manager.h"
#include "calculation.h"
#include "game.h"
#include "input.h"
#include "battlestart.h"
#include "2D_effect.h"
#include "battleresult_Overall.h"
#include "player.h"
#include "skillpoint.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE_WINDOW = "data\\TEXTURE\\battleresult\\resultwindow.png";		// �E�B���h�E�̃e�N�X�`��
	const char* TEXTURE_NUMBER = "data\\TEXTURE\\number\\number_oradano.png";		// �����̃e�N�X�`��
	const char* TEXTURE_RANK[] =	// �����N�̃e�N�X�`��
	{ 
		"data\\TEXTURE\\battleresult\\rank_B.png",	// B
		"data\\TEXTURE\\battleresult\\rank_A.png",	// A
		"data\\TEXTURE\\battleresult\\rank_S.png",	// S
	};
	const MyLib::Vector3 POSITION_RANK[] =
	{
		MyLib::Vector3(990.0f, 270.0f, 0.0f),	// �N���A���Ԃ̈ʒu
		MyLib::Vector3(990.0f, 370.0f, 0.0f),	// ��_���[�W�̈ʒu
		MyLib::Vector3(990.0f, 470.0f, 0.0f),	// ���S�񐔂̈ʒu
	};

	const MyLib::Vector3 POSITION_SCORE[] =
	{
		MyLib::Vector3(880.0f, 237.0f, 0.0f),	// �N���A���Ԃ̈ʒu
		MyLib::Vector3(880.0f, 337.0f, 0.0f),	// ��_���[�W�̈ʒu
		MyLib::Vector3(880.0f, 437.0f, 0.0f),	// ���S�񐔂̈ʒu
	};

	const int DIGIT_NUMBER[] =
	{
		6,	// �N���A����
		4,	// ��_���[�W
		2,	// ���S��
	};
	const D3DXVECTOR2 SIZE_RANK = D3DXVECTOR2(40.0f, 40.0f);
	const D3DXVECTOR2 SIZE_NUMBER = D3DXVECTOR2(22.0f, 22.0f);
	const float DSTANCE_TIMER = 75.0f;

	const float TIME_FADEIN = 0.2f;		// �t�F�[�h�C��
	const float TIME_RANKWAIT = 0.8f;	// �����N����ҋ@
	const float TIME_RANKIN = 0.4f;		// �����N����
	const float TIME_FADEOUT = 0.3f;	// �t�F�[�h�A�E�g
}

//==========================================================================
// �֐����X�g
//==========================================================================
CBattleResult::STATE_FUNC CBattleResult::m_StateFuncList[] =
{
	&CBattleResult::StateFadeIn,			// �t�F�[�h�C��
	&CBattleResult::StateRankWait,			// �����N����҂�
	&CBattleResult::StateRankIn,			// �����N����
	&CBattleResult::StateRankIn_OverAll,	// ���������N����
	&CBattleResult::StateReturnWait,		// �����ҋ@
	&CBattleResult::StateFadeOut,			// �t�F�[�h�A�E�g
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CBattleResult::CBattleResult(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	memset(m_RankInfo, 0, sizeof(m_RankInfo));		// ��ނ��Ƃ̃����N
	memset(m_pRank, 0, sizeof(m_pRank));		// ��ނ��Ƃ̃����N
	memset(m_pClearTime, 0, sizeof(m_pClearTime));		// ��ނ��Ƃ̐���

	m_pOverall = nullptr;	// ����


	m_pDamage = nullptr;			// ��ނ��Ƃ̐���
	m_pDead = nullptr;				// ��ނ��Ƃ̐���
	m_state = STATE::STATE_FADEIN;	// ���
	m_fStateTimer = 0.0f;			// ��ԃ^�C�}�[
	m_fSurvivalTimer = 0.0f;		// �����^�C�}�[
	m_bCompleteStaging = false;		// ���o�����t���O
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CBattleResult::~CBattleResult()
{

}

//==========================================================================
// ��������
//==========================================================================
CBattleResult* CBattleResult::Create()
{
	// �������̊m��
	CBattleResult* pMarker = DEBUG_NEW CBattleResult;

	if (pMarker != nullptr)
	{
		// ����������
		pMarker->Init();
	}

	return pMarker;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CBattleResult::Init()
{

	// �I�u�W�F�N�g2D�̏�����
	CObject2D::Init();

	// �e�N�X�`���ݒ�
	int windowTex = CTexture::GetInstance()->Regist(TEXTURE_WINDOW);
	BindTexture(windowTex);

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(640.0f, 360.0f, 0.0f));

	// �T�C�Y�ݒ�
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(windowTex);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 640.0f);
	SetSize(size);
	SetSizeOrigin(size);

	// �N���A�����N����
	CreateRank();

	// ��������
	CreateClearTime();	// �N���A�^�C������
	CreateDamage();		// ��_���[�W����
	CreateDead();		// ���S�񐔐���
	
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_BATTLERESULT);

	return S_OK;
}

//==========================================================================
// �N���A�����N����
//==========================================================================
void CBattleResult::CreateRank()
{
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();

	// ����̕]�����擾
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		// ����
		m_RankInfo[i].pObj2D = CObject2D::Create(GetPriority());
		if (m_RankInfo[i].pObj2D == nullptr)
		{
			continue;
		}
		CObject2D* pRank = m_RankInfo[i].pObj2D;

		// �����N����o��
		switch (i)
		{
		case CGameRating::RATINGTYPE::RATINGTYPE_TIME:
			m_RankInfo[i].rating = pRating->CalculateClearTimeRank(ratingInfo.clearTime);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DMG:
			m_RankInfo[i].rating = pRating->CalculateRecieveDamageRank(ratingInfo.receiveDamage);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DEAD:
			m_RankInfo[i].rating = pRating->CalculateNumDeadRank(ratingInfo.numDead);
			break;
		}

		// �e�N�X�`���ݒ�
		int rankTex = CTexture::GetInstance()->Regist(TEXTURE_RANK[m_RankInfo[i].rating]);
		pRank->BindTexture(rankTex);

		// �ʒu�ݒ�
		pRank->SetPosition(POSITION_RANK[i]);

		// �T�C�Y�ݒ�
		pRank->SetSize(SIZE_RANK * 3.0f);
		pRank->SetSizeOrigin(pRank->GetSize());

		// �F�ݒ�
		pRank->SetAlpha(0.0f);

	}

}

//==========================================================================
// ���������N����
//==========================================================================
void CBattleResult::CreateRankOverall(int prevPoint)
{
	// �������
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::RATING rating = pRating->CalculateRank(
		m_RankInfo[CGameRating::RATINGTYPE::RATINGTYPE_TIME].rating,
		m_RankInfo[CGameRating::RATINGTYPE::RATINGTYPE_DMG].rating,
		m_RankInfo[CGameRating::RATINGTYPE::RATINGTYPE_DEAD].rating);

	// ����
	m_pOverall = CBattleResult_Overall::Create(rating, prevPoint, prevPoint + pRating->CalculateOverrallRankPoint(rating));

}

//==========================================================================
// �N���A�^�C������
//==========================================================================
void CBattleResult::CreateClearTime()
{
	// ����̕]�����擾
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	for (int i = 0; i < 3; i++)
	{
		// ����
		m_pClearTime[i] = CMultiNumber::Create(
			POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_TIME],
			SIZE_NUMBER,
			2,
			CNumber::EObjectType::OBJECTTYPE_2D,
			TEXTURE_NUMBER, false, GetPriority());
		if (m_pClearTime[i] == nullptr)
		{
			continue;
		}
		CMultiNumber* pNumber = m_pClearTime[i];

		MyLib::Vector3 pos = POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_TIME];

		pos.x -= DSTANCE_TIMER * i;

		// �ʒu�ݒ�
		pNumber->SetPosition(pos);

		// �E�񂹂ɐݒ�
		pNumber->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

		// ���A�b�A�~���b�̌v�Z
		int minutes = static_cast<int>(ratingInfo.clearTime) / (60);
		int seconds = static_cast<int>(ratingInfo.clearTime) % (60);
		int milliseconds = static_cast<int>(ratingInfo.clearTime) % 1000;

		switch (i)
		{
		case 0:
			pNumber->SetValue(milliseconds);
			break;

		case 1:
			pNumber->SetValue(seconds);
			break;

		case 2:
			pNumber->SetValue(minutes);
			break;
		}

		// �F�ݒ�
		pNumber->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	}
}

//==========================================================================
// ��_���[�W����
//==========================================================================
void CBattleResult::CreateDamage()
{
	// ����̕]�����擾
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	// ����
	m_pDamage = CMultiNumber::Create(
		POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DMG],
		SIZE_NUMBER,
		DIGIT_NUMBER[CGameRating::RATINGTYPE::RATINGTYPE_DMG],
		CNumber::EObjectType::OBJECTTYPE_2D,
		TEXTURE_NUMBER, true, GetPriority());
	if (m_pDamage == nullptr)
	{
		return;
	}

	// �E�񂹂ɐݒ�
	m_pDamage->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

	// �l�ݒ�
	m_pDamage->SetValue(ratingInfo.receiveDamage);

	// �F�ݒ�
	m_pDamage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
}

//==========================================================================
// ���S�񐔐���
//==========================================================================
void CBattleResult::CreateDead()
{
	// ����̕]�����擾
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	// ����
	m_pDead = CMultiNumber::Create(
		POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DEAD],
		SIZE_NUMBER,
		DIGIT_NUMBER[CGameRating::RATINGTYPE::RATINGTYPE_DEAD],
		CNumber::EObjectType::OBJECTTYPE_2D,
		TEXTURE_NUMBER, true, GetPriority());
	if (m_pDead == nullptr)
	{
		return;
	}

	// �E�񂹂ɐݒ�
	m_pDead->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

	// �l�ݒ�
	m_pDead->SetValue(ratingInfo.numDead);

	// �F�ݒ�
	m_pDead->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
}

//==========================================================================
// �I������
//==========================================================================
void CBattleResult::Uninit()
{
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_RankInfo[i].pObj2D != nullptr)
		{
			m_RankInfo[i].pObj2D = nullptr;
		}

		if (m_pClearTime[i] != nullptr)
		{
			m_pClearTime[i]->Uninit();
			m_pClearTime[i] = nullptr;
		}
	}

	if (m_pDamage != nullptr)
	{
		m_pDamage->Uninit();
		m_pDamage = nullptr;
	}

	if (m_pDead != nullptr)
	{
		m_pDead->Uninit();
		m_pDead = nullptr;
	}

	/*if (m_pOverall != nullptr)
	{
		m_pOverall->Uninit();
		m_pOverall = nullptr;
	}*/

	CObject2D::Uninit();
}

//==========================================================================
// �폜
//==========================================================================
void CBattleResult::Kill()
{
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_RankInfo[i].pObj2D != nullptr)
		{
			m_RankInfo[i].pObj2D->Uninit();
			m_RankInfo[i].pObj2D = nullptr;
		}

		if (m_pClearTime[i] != nullptr)
		{
			m_pClearTime[i]->Release();
			m_pClearTime[i] = nullptr;
		}
	}

	if (m_pDamage != nullptr)
	{
		m_pDamage->Release();
		m_pDamage = nullptr;
	}

	if (m_pDead != nullptr)
	{
		m_pDead->Release();
		m_pDead = nullptr;
	}

	/*if (m_pOverall != nullptr)
	{
		m_pOverall->Kill();
		m_pOverall = nullptr;
	}*/

	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CBattleResult::Update()
{
	// �^�C�}�[���Z
	m_fStateTimer += CManager::GetInstance()->GetDeltaTime();
	m_fSurvivalTimer += CManager::GetInstance()->GetDeltaTime();

	// ��ԕʏ���
	(this->*(m_StateFuncList[m_state]))();
	if (IsDeath()){
		return;
	}

	if (m_fSurvivalTimer >= 0.3f && !m_bCompleteStaging)
	{
		// ���͏��擾
		CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
		CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

		if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_A, 0) ||
			pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_B, 0) ||
			pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_START, 0) ||
			pInputKeyboard->GetTrigger(DIK_RETURN) ||
			pInputKeyboard->GetTrigger(DIK_SPACE) ||
			pInputKeyboard->GetTrigger(DIK_BACKSPACE)
			)
		{
			AllSetting();
		}
	}

	// ����̕]�����擾
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	// �^�C�}�[�𕪁A�b�A�~���b�ɕϊ�
	int minutes = static_cast<int>(ratingInfo.clearTime / 60);
	int seconds = static_cast<int>(ratingInfo.clearTime) % 60;
	int milliseconds = static_cast<int>((ratingInfo.clearTime - static_cast<int>(ratingInfo.clearTime)) * 1000);
	milliseconds /= 10;

	for (int i = 0; i < 3; i++)
	{
		if (m_pClearTime[i] == nullptr)
		{
			continue;
		}

		int time = 0;

		switch (i)
		{
		case 0:
			m_pClearTime[i]->SetValue(milliseconds);
			break;

		case 1:
			m_pClearTime[i]->SetValue(seconds);
			break;

		case 2:
			m_pClearTime[i]->SetValue(minutes);
			break;
		}

		MyLib::Vector3 pos = POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_TIME];

		pos.x -= DSTANCE_TIMER * i;
		m_pClearTime[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, GetAlpha()));
		m_pClearTime[i]->Update();
		m_pClearTime[i]->SetPosition(pos);
	}

	m_pDamage->SetValue(ratingInfo.receiveDamage);
	m_pDamage->SetPosition(POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DMG]);
	m_pDamage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, GetAlpha()));
	m_pDamage->Update();

	m_pDead->SetValue(ratingInfo.numDead);
	m_pDead->SetPosition(POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DEAD]);
	m_pDead->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, GetAlpha()));
	m_pDead->Update();

	// �X�V����
	CObject2D::Update();
}

//==========================================================================
// �S�Đݒ�
//==========================================================================
void CBattleResult::AllSetting()
{


	// �s�����x�ݒ�
	SetAlpha(1.0f);

	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_RankInfo[i].pObj2D == nullptr)
		{
			continue;
		}
		CObject2D* pRank = m_RankInfo[i].pObj2D;
		pRank->SetSize(SIZE_RANK);
		pRank->SetAlpha(1.0f);
	}

	if (m_pOverall == nullptr)
	{
		// �O��|�C���g�擾
		CListManager<CPlayer> playerList = CPlayer::GetListObj();
		CPlayer* pPlayer = playerList.GetData(0);
		CSkillPoint* pSkillPoint = pPlayer->GetSkillPoint();

		// ������ʐ���
		CreateRankOverall(pSkillPoint->GetPoint());
	}
	m_pOverall->AllSetting();

	// ���o�����t���O
	m_bCompleteStaging = true;
	m_state = STATE::STATE_RETURNWAIT;
	m_fStateTimer = 0.0f;
}

//==========================================================================
// �t�F�[�h�C��
//==========================================================================
void CBattleResult::StateFadeIn()
{
	float alpha = UtilFunc::Correction::EasingLinear(0.0f, 1.0f, 0.0f, TIME_FADEIN, m_fStateTimer);
	SetAlpha(alpha);

	if (m_fStateTimer >= TIME_FADEIN)
	{
		m_state = STATE::STATE_RANKWAIT;
		m_fStateTimer = 0.0f;
		SetAlpha(1.0f);
		return;
	}
}

//==========================================================================
// �����N����҂�
//==========================================================================
void CBattleResult::StateRankWait()
{
	if (m_fStateTimer >= TIME_RANKWAIT)
	{
		m_state = STATE::STATE_RANKIN;
		m_fStateTimer = 0.0f;
		return;
	}
}

//==========================================================================
// �����N����
//==========================================================================
void CBattleResult::StateRankIn()
{
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_RankInfo[i].pObj2D == nullptr)
		{
			continue;
		}
		CObject2D* pRank = m_RankInfo[i].pObj2D;

		// �T�C�Y�擾
		D3DXVECTOR2 size = pRank->GetSize();
		D3DXVECTOR2 sizeOrigin = pRank->GetSizeOrigin();
		D3DXVECTOR2 destSize = UtilFunc::Transformation::AdjustSizeByWidth(size, SIZE_RANK.x);

		// ���X�Ɍ���
		float len = UtilFunc::Correction::EasingEaseOut(sizeOrigin.x, SIZE_RANK.x, 0.0f, TIME_RANKIN, m_fStateTimer);
		pRank->SetSize(D3DXVECTOR2(len, len));


		float alpha = UtilFunc::Correction::EasingLinear(0.0f, 1.0f, 0.0f, TIME_RANKIN, m_fStateTimer);
		pRank->SetAlpha(alpha);
	}

	if (m_fStateTimer >= TIME_RANKIN)
	{
		m_state = STATE::STATE_RANKIN_OVERALL;
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_BATTLERESULT_RANK);

		for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
		{
			if (m_RankInfo[i].pObj2D == nullptr)
			{
				continue;
			}
			CObject2D* pRank = m_RankInfo[i].pObj2D;
			pRank->SetSize(SIZE_RANK);
			pRank->SetAlpha(1.0f);
		}
		return;
	}
}

//==========================================================================
// ���������N����
//==========================================================================
void CBattleResult::StateRankIn_OverAll()
{
	if (m_fStateTimer >= 0.5f)
	{
		// �O��|�C���g�擾
		CListManager<CPlayer> playerList = CPlayer::GetListObj();
		CPlayer* pPlayer = playerList.GetData(0);
		CSkillPoint* pSkillPoint = pPlayer->GetSkillPoint();

		// ������ʐ���
		CreateRankOverall(pSkillPoint->GetPoint());
		m_state = STATE::STATE_RETURNWAIT;
		m_fStateTimer = 0.0f;
	}
}

//==========================================================================
// �����ҋ@
//==========================================================================
void CBattleResult::StateReturnWait()
{
	//m_fStateTimer = 0.0f;

	if (m_fStateTimer >= 0.4f)
	{
		for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
		{
			if (m_RankInfo[i].pObj2D == nullptr ||
				m_RankInfo[i].rating != CGameRating::RATING::RATING_S)
			{
				continue;
			}
			CObject2D* pRank = m_RankInfo[i].pObj2D;

			MyLib::Vector3 pos = pRank->GetPosition();
			pos.x += UtilFunc::Transformation::Random(-20, 20);
			pos.y += UtilFunc::Transformation::Random(-20, 20);

			D3DXCOLOR col;
			if (UtilFunc::Transformation::Random(0, 1) == 0) {
				col = D3DXCOLOR(0.9f, 0.9f, 1.0f, 0.4f);
			}
			else {
				col = D3DXCOLOR(0.2f, 0.2f, 1.0f, 0.4f);
			}

			// �G�t�F�N�g�̐ݒ�
			CEffect2D* pEffect = CEffect2D::Create(pos, 0.0f, col, static_cast<float>(UtilFunc::Transformation::Random(10, 30)),
				20,
				CEffect2D::MOVEEFFECT::MOVEEFFECT_SUB, CEffect2D::TYPE::TYPE_JUJI3);
			pEffect->SetRotation(MyLib::Vector3(0.0f, 0.0f, UtilFunc::Transformation::GetRandomPi()));
		}

		m_fStateTimer = UtilFunc::Transformation::Random(0, 30) * 0.01f;
	}

	// ���͏��擾
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	if (m_bCompleteStaging &&
		(pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_A, 0) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_B, 0) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_START, 0) ||
		pInputKeyboard->GetTrigger(DIK_RETURN) ||
		pInputKeyboard->GetTrigger(DIK_SPACE) ||
		pInputKeyboard->GetTrigger(DIK_BACKSPACE)
		))
	{
		m_state = STATE::STATE_FADEOUT;
		m_fStateTimer = 0.0f;

		m_pOverall->SetState(CBattleResult_Overall::STATE::STATE_FADEOUT);

		CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SceneType::SCENE_MAINCLEAR);
		CGame::GetInstance()->GetGameManager()->GameClearSettings();
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_BATTLERESULT_END);
	}

	if (m_pOverall->IsCompleteStaging())
	{
		m_bCompleteStaging = true;
	}
}

//==========================================================================
// �t�F�[�h�A�E�g
//==========================================================================
void CBattleResult::StateFadeOut()
{
	float alpha = UtilFunc::Correction::EasingLinear(1.0f, 0.0f, 0.0f, TIME_FADEOUT, m_fStateTimer);
	SetAlpha(alpha);

	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_RankInfo[i].pObj2D == nullptr)
		{
			continue;
		}
		CObject2D* pRank = m_RankInfo[i].pObj2D;
		pRank->SetAlpha(alpha);
	}

	if (m_fStateTimer >= TIME_FADEOUT)
	{
		Kill();
		return;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CBattleResult::Draw()
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// �I�u�W�F�N�g2D�̕`��
	CObject2D::Draw();

	// �A���t�@�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}
