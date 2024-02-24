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
	const D3DXVECTOR2 SIZE_NUMBER = D3DXVECTOR2(22.0f, 22.0f);
	const float DSTANCE_TIMER = 75.0f;
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CBattleResult::CBattleResult(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	memset(m_pRank, 0, sizeof(m_pRank));		// ��ނ��Ƃ̃����N
	memset(m_pNumber, 0, sizeof(m_pNumber));	// ��ނ��Ƃ̐���
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
		m_pRank[i] = CObject2D::Create(GetPriority());
		if (m_pRank[i] == nullptr)
		{
			continue;
		}
		CObject2D* pRank = m_pRank[i];

		// ����̃����N
		CGameRating::RATING rating;

		// �����N����o��
		switch (i)
		{
		case CGameRating::RATINGTYPE::RATINGTYPE_TIME:
			rating = pRating->CalculateClearTimeRank(ratingInfo.clearTime);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DMG:
			rating = pRating->CalculateRecieveDamageRank(ratingInfo.receiveDamage);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DEAD:
			rating = pRating->CalculateNumDeadRank(ratingInfo.numDead);
			break;
		}

		// �e�N�X�`���ݒ�
		int rankTex = CTexture::GetInstance()->Regist(TEXTURE_RANK[rating]);
		pRank->BindTexture(rankTex);

		// �ʒu�ݒ�
		pRank->SetPosition(POSITION_RANK[i]);

		// �T�C�Y�ݒ�
		pRank->SetSize(D3DXVECTOR2(40.0f, 40.0f));
		SetSizeOrigin(pRank->GetSize());
	}

}

#if 0
//==========================================================================
// ��������
//==========================================================================
void CBattleResult::CreateNumber()
{

	// ����̕]�����擾
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		// ����
		m_pNumber[i] = CMultiNumber::Create(
			POSITION_SCORE[i],
			D3DXVECTOR2(25.0f, 25.0f),
			DIGIT_NUMBER[i],
			CNumber::EObjectType::OBJECTTYPE_2D,
			TEXTURE_NUMBER, true, GetPriority());
		if (m_pNumber[i] == nullptr)
		{
			continue;
		}
		CMultiNumber* pNumber = m_pNumber[i];

		// �ʒu�ݒ�
		pNumber->SetPosition(POSITION_SCORE[i]);

		// �E�񂹂ɐݒ�
		pNumber->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

		switch (i)
		{
		case CGameRating::RATINGTYPE::RATINGTYPE_TIME:
			pNumber->SetValue(ratingInfo.clearTime);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DMG:
			pNumber->SetValue(ratingInfo.numDead);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DEAD:
			pNumber->SetValue(ratingInfo.numDead);
			break;
		}
	}
}
#endif

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

	// �ʒu�ݒ�
	m_pDamage->SetPosition(POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DMG]);

	// �E�񂹂ɐݒ�
	m_pDamage->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

	// �l�ݒ�
	m_pDamage->SetValue(ratingInfo.receiveDamage);
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
}

//==========================================================================
// �I������
//==========================================================================
void CBattleResult::Uninit()
{
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_pRank[i] != nullptr)
		{
			m_pRank[i] = nullptr;
		}

		if (m_pNumber[i] != nullptr)
		{
			m_pNumber[i]->Uninit();
			m_pNumber[i] = nullptr;
		}
	}

	CObject2D::Uninit();
}

//==========================================================================
// �폜
//==========================================================================
void CBattleResult::Kill()
{
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_pRank[i] != nullptr)
		{
			m_pRank[i]->Uninit();
			m_pRank[i] = nullptr;
		}

		if (m_pNumber[i] != nullptr)
		{
			m_pNumber[i]->Release();
			m_pNumber[i] = nullptr;
		}
	}

	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CBattleResult::Update()
{

	// ����̕]�����擾
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();
#if 0
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_pNumber[i] != nullptr)
		{
			switch (i)
			{
			case CGameRating::RATINGTYPE::RATINGTYPE_TIME:
			{
				// ���A�b�A�~���b�̌v�Z
				int minutes = static_cast<int>(m_fTime) / (60);
				int seconds = (static_cast<int>(m_fTime) % (60));
				int milliseconds = static_cast<int>(m_fTime) % 1000;

				int time = minutes * 10000 + seconds * 100 + milliseconds;
				m_pNumber[i]->SetValue(time);
			}
				break;

			case CGameRating::RATINGTYPE::RATINGTYPE_DMG:
				m_pNumber[i]->SetValue(ratingInfo.receiveDamage);
				break;

			case CGameRating::RATINGTYPE::RATINGTYPE_DEAD:
				m_pNumber[i]->SetValue(ratingInfo.numDead);
				break;
			}
		}
		m_pNumber[i]->SetPosition(POSITION_SCORE[i]);
		m_pNumber[i]->Update();

	}
#endif

	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			m_pClearTime[i]->SetValue(static_cast<int>(ratingInfo.clearTime) % 1000);
			break;

		case 1:
			m_pClearTime[i]->SetValue(static_cast<int>(ratingInfo.clearTime) % (60));
			break;

		case 2:
			m_pClearTime[i]->SetValue(static_cast<int>(ratingInfo.clearTime) / (60));
			break;
		}

		MyLib::Vector3 pos = POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_TIME];

		pos.x -= DSTANCE_TIMER * i;
		m_pClearTime[i]->Update();
		m_pClearTime[i]->SetPosition(pos);
	}

	m_pDamage->SetValue(ratingInfo.receiveDamage);
	m_pDamage->SetPosition(POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DMG]);
	m_pDamage->Update();
	m_pDead->SetValue(ratingInfo.numDead);
	m_pDead->SetPosition(POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DEAD]);
	m_pDead->Update();

	// �X�V����
	CObject2D::Update();
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
