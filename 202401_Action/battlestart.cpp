//=============================================================================
// 
//  �퓬�J�n���� [battlestart.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "battlestart.h"
#include "battlestart_effect.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "particle.h"
#include "game.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* SWORD_TEXTURE = "data\\TEXTURE\\battlestart\\sword.png";				// �e�N�X�`���̃t�@�C��
	const char* SWORD_TEXTURE_COMPLETE = "data\\TEXTURE\\battlestart\\sword_complete.png";				// �e�N�X�`���̃t�@�C��
	const char* TEXT_TEXTURE_COMPLETE = "data\\TEXTURE\\battlestart\\battlestart_complete.png";				// �e�N�X�`���̃t�@�C��
	const char* TEXT_TEXTURE = "data\\TEXTURE\\battlestart\\battlestart.png";	// �e�N�X�`���̃t�@�C��
	const float TIME_GETTOGETHER = 0.6f;	// �W�܂鎞��
	const float TIME_CHARGE = 1.0f;			// ���ߎ���
	const float TIME_STINGS = 0.05f;		// �h���鎞��
	const float TIME_SCALE = 0.7f;			// �g�k����
	const float TIME_FADEOUT = 0.3f;		// �t�F�[�h�A�E�g����
	const MyLib::Vector3 DESTROTATION_GETTOGETHER = MyLib::Vector3(0.0f, 0.0f, D3DX_PI * 0.75f);
	const MyLib::Vector3 DESTPOSITION_GETTOGETHER = MyLib::Vector3(130.0f, 230.0f, 0.0f);
	const MyLib::Vector3 DESTPOSITION_STINGS = MyLib::Vector3(0.0f, 360.0f, 0.0f);
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CBattleStart::STATE_FUNC CBattleStart::m_StateFuncList[] =
{
	&CBattleStart::StateGetTogether,	// �W�܂�
	&CBattleStart::StateCharge,			// ����
	&CBattleStart::StateStings,			// �h����
	&CBattleStart::StateScale,			// �g�k
	&CBattleStart::StateFadeOut,		// �t�F�[�h�A�E�g
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CBattleStart::CBattleStart(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_state = STATE::STATE_GETTOGETHER;		// ���
	m_fStateTime = 0.0f;	// ��ԃ^�C�}�[
	memset(m_RockOnInfo, NULL, sizeof(m_RockOnInfo));
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CBattleStart::~CBattleStart()
{

}

//==========================================================================
// ��������
//==========================================================================
CBattleStart* CBattleStart::Create(const MyLib::Vector3& pos)
{
	// �������̊m��
	CBattleStart* pMarker = DEBUG_NEW CBattleStart;

	if (pMarker != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// �ʒu�ݒ�
		pMarker->SetPosition(pos);
		pMarker->SetOriginPosition(pos);

		// ����������
		pMarker->Init();
	}

	return pMarker;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CBattleStart::Init()
{

	CTexture::GetInstance()->Regist(TEXT_TEXTURE);
	CTexture::GetInstance()->Regist(TEXT_TEXTURE_COMPLETE);
	CTexture::GetInstance()->Regist(SWORD_TEXTURE);
	CTexture::GetInstance()->Regist(SWORD_TEXTURE_COMPLETE);

	// �I�u�W�F�N�g2D�̏�����
	CObject2D::Init();
	int pointTex = CTexture::GetInstance()->Regist(TEXT_TEXTURE);
	BindTexture(pointTex);
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(pointTex);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 240.0f);
	SetSize(D3DXVECTOR2(size.x, 0.0f));
	SetSizeOrigin(size);

	// ��ނ̐ݒ�
	SetType(CObject::TYPE::TYPE_OBJECT2D);

	int nTexIdx = CTexture::GetInstance()->Regist(SWORD_TEXTURE);

	size = CTexture::GetInstance()->GetImageSize(nTexIdx);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 120.0f);

	MyLib::Vector3 pos = GetPosition();

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// ��������
		m_RockOnInfo[nCntGauge].p2D = CObject2D::Create();
		if (m_RockOnInfo[nCntGauge].p2D == nullptr)
		{
			return E_FAIL;
		}

		CObject2D* pObj = m_RockOnInfo[nCntGauge].p2D;

		// �e�N�X�`�����蓖��
		pObj->BindTexture(nTexIdx);

		// ��ނ̐ݒ�
		pObj->SetType(CObject::TYPE_NONE);

		// ���ݒ�
		pObj->SetSize(size);

		m_RockOnInfo[nCntGauge].Angle = 1;
		float angle = 0.0f;

		if (nCntGauge % 2 == 0)
		{
			angle += D3DX_PI;
			m_RockOnInfo[nCntGauge].Angle = -1;
		}

		
		pObj->SetRotation(MyLib::Vector3(0.0f, 0.0f, angle));
		pObj->SetOriginRotation(pObj->GetRotation());

		pObj->SetPosition(MyLib::Vector3(640.0f + 640.0f * m_RockOnInfo[nCntGauge].Angle, pos.y, 0.0f));
		pObj->SetOriginPosition(pObj->GetPosition());
	}


	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CBattleStart::Uninit()
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_RockOnInfo[nCntGauge].p2D->Uninit();
		m_RockOnInfo[nCntGauge].p2D = nullptr;
	}

	CObject2D::Uninit();
}

//==========================================================================
// �I������
//==========================================================================
void CBattleStart::Kill()
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_RockOnInfo[nCntGauge].p2D != nullptr)
		{
			// �I������
			m_RockOnInfo[nCntGauge].p2D->Uninit();
			m_RockOnInfo[nCntGauge].p2D = nullptr;
		}
	}

	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CBattleStart::Update()
{
	// ��ԃ^�C�}�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	// ��ԕʏ���
	(this->*(m_StateFuncList[m_state]))();

	if (IsDeath())
	{
		return;
	}

	for (auto& info : m_RockOnInfo)
	{
		info.p2D->Update();
	}

	CObject2D::Update();
}

//==========================================================================
// �W�܂�
//==========================================================================
void CBattleStart::StateGetTogether()
{
	if (m_fStateTime >= TIME_GETTOGETHER)
	{
		m_fStateTime = 0.0f;
		m_state = STATE_CHARGE;

		SetSize(GetSizeOrigin());
		for (const auto& info : m_RockOnInfo)
		{
			info.p2D->SetOriginPosition(info.p2D->GetPosition());
		}
		return;
	}

	float ratio = m_fStateTime / TIME_GETTOGETHER;

	// �T�C�Y�ݒ�
	D3DXVECTOR2 size = GetSize();
	size.y = UtilFunc::Correction::EasingEaseIn(0.0f, GetSizeOrigin().y, ratio);
	SetSize(size);

	float destAngle = DESTROTATION_GETTOGETHER.z;
	for (const auto& info : m_RockOnInfo)
	{
		MyLib::Vector3 setpos = info.p2D->GetPosition();
		MyLib::Vector3 posOrigin = info.p2D->GetOriginPosition();
		MyLib::Vector3 rot = info.p2D->GetRotation();
		MyLib::Vector3 rotOrigin = info.p2D->GetOriginRotation();

		UtilFunc::Transformation::RotNormalize(destAngle);
		rot.z = UtilFunc::Correction::EasingEaseIn(rotOrigin.z, destAngle, ratio);
		UtilFunc::Transformation::RotNormalize(rot.z);

		setpos.x = UtilFunc::Correction::EasingEaseIn(posOrigin.x, 640.0f + (DESTPOSITION_GETTOGETHER.x * info.Angle), ratio);
		setpos.y = UtilFunc::Correction::EasingEaseIn(posOrigin.y, DESTPOSITION_GETTOGETHER.y, ratio);

		info.p2D->SetRotation(rot);
		info.p2D->SetPosition(setpos);

		destAngle += D3DX_PI + D3DX_PI * 0.5f;
	}
}

//==========================================================================
// ����
//==========================================================================
void CBattleStart::StateCharge()
{
	if (m_fStateTime >= TIME_CHARGE)
	{
		m_fStateTime = 0.0f;
		m_state = STATE_STINGS;

		for (const auto& info : m_RockOnInfo)
		{
			info.p2D->SetOriginPosition(info.p2D->GetPosition());
		}
		return;
	}

	for (const auto& info : m_RockOnInfo)
	{
		MyLib::Vector3 pos = info.p2D->GetPosition();
		MyLib::Vector3 rot = info.p2D->GetRotation();

		pos.x += sinf(D3DX_PI * 0.5f + rot.z) * 0.5f;
		pos.y += cosf(D3DX_PI * 0.5f + rot.z) * 0.5f;

		info.p2D->SetPosition(pos);
	}
}

//==========================================================================
// �h����
//==========================================================================
void CBattleStart::StateStings()
{
	if (m_fStateTime >= TIME_STINGS)
	{
		m_fStateTime = 0.0f;
		m_state = STATE_SCALE;


		// ������̃e�N�X�`���ɐؑ�
		int nTexIdx = CTexture::GetInstance()->Regist(TEXT_TEXTURE_COMPLETE);
		BindTexture(nTexIdx);

		nTexIdx = CTexture::GetInstance()->Regist(SWORD_TEXTURE_COMPLETE);

		for (const auto& info : m_RockOnInfo)
		{
			MyLib::Vector3 setpos = info.p2D->GetPosition();
			MyLib::Vector3 posOrigin = info.p2D->GetOriginPosition();

			setpos.x = UtilFunc::Correction::EasingEaseIn(posOrigin.x, 640.0f + (DESTPOSITION_STINGS.x * info.Angle), 1.0f);
			setpos.y = UtilFunc::Correction::EasingEaseIn(posOrigin.y, DESTPOSITION_STINGS.y, 1.0f);

			info.p2D->SetPosition(setpos);
			info.p2D->BindTexture(nTexIdx);
		}

		// �h����p�[�e�B�N������
		my_particle::Create(GetPosition(), my_particle::TYPE::TYPE_BATTLESTART);

		// �����G�t�F�N�g����
		CBattleStart_Effect::Create(GetPosition());
		return;
	}

	float ratio = m_fStateTime / TIME_STINGS;
	for (const auto& info : m_RockOnInfo)
	{
		MyLib::Vector3 setpos = info.p2D->GetPosition();
		MyLib::Vector3 posOrigin = info.p2D->GetOriginPosition();

		setpos.x = UtilFunc::Correction::EasingEaseIn(posOrigin.x, 640.0f + (DESTPOSITION_STINGS.x * info.Angle), ratio);
		setpos.y = UtilFunc::Correction::EasingEaseIn(posOrigin.y, DESTPOSITION_STINGS.y, ratio);

		info.p2D->SetPosition(setpos);
	}
}

//==========================================================================
// �g�k
//==========================================================================
void CBattleStart::StateScale()
{
	if (m_fStateTime >= TIME_SCALE)
	{
		m_fStateTime = 0.0f;
		m_state = STATE_FADEOUT;
	}
}

//==========================================================================
// �t�F�[�h�A�E�g
//==========================================================================
void CBattleStart::StateFadeOut()
{
	if (m_fStateTime >= TIME_FADEOUT)
	{
		m_fStateTime = 0.0f;
		Uninit();

		// �퓬�J�n�ɑJ��
		CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SceneType::SCENE_MAIN);
		return;
	}
	float ratio = 1.0f - m_fStateTime / TIME_FADEOUT;

	// �s�����x�ݒ�
	SetAlpha(ratio);
	for (const auto& info : m_RockOnInfo)
	{
		info.p2D->SetAlpha(ratio);
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CBattleStart::Draw()
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// �`�揈��
	for (const auto& info : m_RockOnInfo)
	{
		info.p2D->Draw();
	}

	// �I�u�W�F�N�g2D�̕`��
	CObject2D::Draw();

	// �A���t�@�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}
