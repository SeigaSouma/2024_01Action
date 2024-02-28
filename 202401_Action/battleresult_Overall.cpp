//=============================================================================
// 
//  ������ʏ��� [battleresult_Overall.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "battleresult_Overall.h"
#include "manager.h"
#include "calculation.h"
#include "game.h"
#include "input.h"
#include "2D_effect.h"
#include "player.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE_NUMBER = "data\\TEXTURE\\number\\number_oradano.png";		// �����̃e�N�X�`��
	const char* TEXTURE_RANK[] =	// �����N�̃e�N�X�`��
	{ 
		"data\\TEXTURE\\battleresult\\rank_B.png",	// B
		"data\\TEXTURE\\battleresult\\rank_A.png",	// A
		"data\\TEXTURE\\battleresult\\rank_S.png",	// S
	};
	const MyLib::Vector3 POSITION_RANK = MyLib::Vector3(535.0f, 560.0f, 0.0f);

	const MyLib::Vector3 POSITION_POINT[] =
	{
		MyLib::Vector3(652.0f, 600.0f, 0.0f),	// �N���A���Ԃ̈ʒu
		MyLib::Vector3(922.0f, 600.0f, 0.0f),	// ��_���[�W�̈ʒu
	};

	const D3DXVECTOR2 SIZE_RANK = D3DXVECTOR2(70.0f, 70.0f);
	const D3DXVECTOR2 SIZE_NUMBER = D3DXVECTOR2(30.0f, 30.0f);

	const float TIME_FADEIN = 0.4f;		// �t�F�[�h�C��
	const float TIME_WAIT_POINT = 0.5f;	// �|�C���g����ҋ@
	const float TIME_FADEIN_POINT = 0.3f;		// �t�F�[�h�C��
	const float TIME_FADEOUT = 0.3f;	// �t�F�[�h�A�E�g
}

//==========================================================================
// �֐����X�g
//==========================================================================
CBattleResult_Overall::STATE_FUNC CBattleResult_Overall::m_StateFuncList[] =
{
	&CBattleResult_Overall::StateNone,		// �Ȃɂ��Ȃ�
	&CBattleResult_Overall::StateFadeIn,	// �t�F�[�h�C��
	&CBattleResult_Overall::StateRankWait,	// �|�C���g����҂�
	&CBattleResult_Overall::StateFadeIn_Point,	// �t�F�[�h�C��
	&CBattleResult_Overall::StateFadeOut,	// �t�F�[�h�A�E�g
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CBattleResult_Overall::CBattleResult_Overall(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_state = STATE::STATE_FADEIN;	// ���
	m_fStateTimer = 0.0f;			// ��ԃ^�C�}�[
	m_bCompleteStaging = false;		// ���o�����t���O
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CBattleResult_Overall::~CBattleResult_Overall()
{

}

//==========================================================================
// ��������
//==========================================================================
CBattleResult_Overall* CBattleResult_Overall::Create(CGameRating::RATING rank, int prevPoint, int currentPoint)
{
	// �������̊m��
	CBattleResult_Overall* pMarker = DEBUG_NEW CBattleResult_Overall;

	if (pMarker != nullptr)
	{
		pMarker->m_nPoint[NUMBERTYPE::NUMBERTYPE_PREV] = prevPoint;			// �O��|�C���g
		pMarker->m_nPoint[NUMBERTYPE::NUMBERTYPE_CURRENT] = currentPoint;	// ����|�C���g
		pMarker->m_OverallRating = rank;

		// ����������
		pMarker->Init();
	}

	return pMarker;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CBattleResult_Overall::Init()
{

	// �I�u�W�F�N�g2D�̏�����
	CObject2D::Init();

	// �e�N�X�`���ݒ�
	int windowTex = CTexture::GetInstance()->Regist(TEXTURE_RANK[m_OverallRating]);
	BindTexture(windowTex);

	// �ʒu�ݒ�
	SetPosition(POSITION_RANK);

	// �T�C�Y�ݒ�
	SetSize(SIZE_RANK);
	SetSizeOrigin(SIZE_RANK * 3.0f);

	// �|�C���g��������
	CreatePoint();

	// ��Ԑݒ�
	m_state = STATE_FADEIN;
	m_fStateTimer = 0.0f;
	SetAlpha(0.0f);

	return S_OK;
}

//==========================================================================
// �|�C���g��������
//==========================================================================
void CBattleResult_Overall::CreatePoint()
{
	// ����̕]�����擾
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		// ����
		m_pDiffPoint[i] = CMultiNumber::Create(
			POSITION_POINT[i],
			SIZE_NUMBER,
			2,
			CNumber::EObjectType::OBJECTTYPE_2D,
			TEXTURE_NUMBER, false, GetPriority());
		if (m_pDiffPoint[i] == nullptr)
		{
			continue;
		}
		CMultiNumber* pNumber = m_pDiffPoint[i];

		// �E�񂹂ɐݒ�
		pNumber->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

		pNumber->SetValue(m_nPoint[i]);

		// �F�ݒ�
		pNumber->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	}
}

//==========================================================================
// �I������
//==========================================================================
void CBattleResult_Overall::Uninit()
{
	
	CObject2D::Uninit();
}

//==========================================================================
// �폜
//==========================================================================
void CBattleResult_Overall::Kill()
{
	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		if (m_pDiffPoint[i] != nullptr)
		{
			m_pDiffPoint[i]->Release();
			m_pDiffPoint[i] = nullptr;
		}
	}

	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CBattleResult_Overall::Update()
{
	// ��ԃ^�C�}�[���Z
	m_fStateTimer += CManager::GetInstance()->GetDeltaTime();

	// ��ԕʏ���
	(this->*(m_StateFuncList[m_state]))();
	if (IsDeath()){
		return;
	}

	// ����̕]�����擾
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		if (m_pDiffPoint[i] == nullptr)
		{
			continue;
		}

		m_pDiffPoint[i]->Update();
	}

	// �X�V����
	CObject2D::Update();
}

//==========================================================================
// �S�Đݒ�
//==========================================================================
void CBattleResult_Overall::AllSetting()
{
	SetAlpha(1.0f);
	SetSize(SIZE_RANK);

	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		if (m_pDiffPoint[i] == nullptr)
		{
			continue;
		}
		// �F�ݒ�
		m_pDiffPoint[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

	// ���o�����t���O
	m_bCompleteStaging = true;
	m_state = STATE::STATE_NONE;
	m_fStateTimer = 0.0f;
}

//==========================================================================
// �����Ȃ�
//==========================================================================
void CBattleResult_Overall::StateNone()
{
	if (m_fStateTimer >= 0.4f)
	{
		if (m_OverallRating == CGameRating::RATING::RATING_S)
		{
			MyLib::Vector3 pos = GetPosition();
			pos.x += UtilFunc::Transformation::Random(-40, 40);
			pos.y += UtilFunc::Transformation::Random(-40, 40);

			D3DXCOLOR col;
			if (UtilFunc::Transformation::Random(0, 1) == 0) {
				col = D3DXCOLOR(0.9f, 0.9f, 1.0f, 0.5f);
			}
			else {
				col = D3DXCOLOR(0.1f, 0.1f, 1.0f, 0.5f);
			}

			// �G�t�F�N�g�̐ݒ�
			CEffect2D* pEffect = CEffect2D::Create(pos, 0.0f, col, static_cast<float>(UtilFunc::Transformation::Random(40, 60)),
				20,
				CEffect2D::MOVEEFFECT::MOVEEFFECT_SUB, CEffect2D::TYPE::TYPE_JUJI3);
			pEffect->SetRotation(MyLib::Vector3(0.0f, 0.0f, UtilFunc::Transformation::GetRandomPi()));
		}

		m_fStateTimer = UtilFunc::Transformation::Random(0, 30) * 0.01f;
	}
}

//==========================================================================
// �t�F�[�h�C��
//==========================================================================
void CBattleResult_Overall::StateFadeIn()
{
	// �T�C�Y�擾
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2 sizeOrigin = GetSizeOrigin();
	D3DXVECTOR2 destSize = UtilFunc::Transformation::AdjustSizeByWidth(size, SIZE_RANK.x);

	// ���X�Ɍ���
	float len = UtilFunc::Correction::EasingEaseOut(sizeOrigin.x, SIZE_RANK.x, 0.0f, TIME_FADEIN, m_fStateTimer);
	SetSize(D3DXVECTOR2(len, len));

	// �s�����x�ݒ�
	float alpha = UtilFunc::Correction::EasingLinear(0.0f, 1.0f, 0.0f, TIME_FADEIN, m_fStateTimer);
	SetAlpha(alpha);

	if (m_fStateTimer >= TIME_FADEIN)
	{
		m_state = STATE::STATE_WAIT_POINT;
		m_fStateTimer = 0.0f;
		SetAlpha(1.0f);
		SetSize(SIZE_RANK);
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_BATTLERESULT_RANK);
		return;
	}
}

//==========================================================================
// �����N����҂�
//==========================================================================
void CBattleResult_Overall::StateRankWait()
{
	if (m_fStateTimer >= TIME_WAIT_POINT)
	{
		m_state = STATE::STATE_FADEIN_POINT;
		m_fStateTimer = 0.0f;
	}
}

//==========================================================================
// �|�C���g�t�F�[�h�C��
//==========================================================================
void CBattleResult_Overall::StateFadeIn_Point()
{
	// �s�����x�ݒ�
	float alpha = UtilFunc::Correction::EasingLinear(0.0f, 1.0f, 0.0f, TIME_FADEIN_POINT, m_fStateTimer);

	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		if (m_pDiffPoint[i] == nullptr)
		{
			continue;
		}
		CMultiNumber* pNumber = m_pDiffPoint[i];

		// �F�ݒ�
		pNumber->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha));
	}

	if (m_fStateTimer >= TIME_FADEIN_POINT)
	{
		m_state = STATE::STATE_NONE;
		m_fStateTimer = 0.0f;

		for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
		{
			if (m_pDiffPoint[i] == nullptr)
			{
				continue;
			}
			// �F�ݒ�
			m_pDiffPoint[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		}

		// ���o�����t���O
		m_bCompleteStaging = true;
		return;
	}
}

//==========================================================================
// �t�F�[�h�A�E�g
//==========================================================================
void CBattleResult_Overall::StateFadeOut()
{
	float alpha = UtilFunc::Correction::EasingLinear(1.0f, 0.0f, 0.0f, TIME_FADEOUT, m_fStateTimer);
	SetAlpha(alpha);

	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		if (m_pDiffPoint[i] == nullptr)
		{
			continue;
		}
		// �F�ݒ�
		m_pDiffPoint[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha));
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
void CBattleResult_Overall::Draw()
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

//==========================================================================
// �`�揈��
//==========================================================================
void CBattleResult_Overall::SetState(STATE state)
{
	m_state = state;
	m_fStateTimer = 0.0f;
}
