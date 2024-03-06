//=============================================================================
// 
//  �I�u�W�F�N�g�L�����N�^�[���� [objectChara.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "objectChara.h"
#include "calculation.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "3D_effect.h"

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CObjectChara::CObjectChara(int nPriority) : CObjectHierarchy(nPriority)
{
	// �l�̃N���A
	m_fVelocity = 0.0f;		// �ړ����x
	m_fRotDest = 0.0f;		// �ڕW�̌���
	m_nLife = 0;			// �̗�
	m_nLifeOrigin = 0;		// ���̗̑�
	m_nMotionStartIdx = 0;	// ���[�V�����J�n�̃C���f�b�N�X�ԍ�
	m_nAddScore = 0;		// �X�R�A���Z��
	m_bInDicision = false;	// �U�����蒆�t���O
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CObjectChara::~CObjectChara()
{

}

//==========================================================================
// ��������
//==========================================================================
CObjectChara* CObjectChara::Create(const std::string pTextFile)
{
	// �������̊m��
	CObjectChara* pObjChara = DEBUG_NEW CObjectChara;

	if (pObjChara != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		HRESULT hr = pObjChara->SetCharacter(pTextFile);
		if (FAILED(hr))
		{// ���s���Ă�����
			return nullptr;
		}
	}

	return pObjChara;
}

//==========================================================================
// �L�����쐬
//==========================================================================
HRESULT CObjectChara::SetCharacter(const std::string pTextFile)
{
	// �L�����쐬
	CObjectHierarchy::SetCharacter(pTextFile);

	// ���[�V�����̐�������
	m_pMotion = CMotion::Create(pTextFile, this);

	// ���[�V�����̐ݒ�
	if (m_pMotion != nullptr)
	{
		m_pMotion->SetModel(GetModel(), GetNumModel());

		// �|�[�Y�̃��Z�b�g
		m_pMotion->ResetPose(0);
	}

	// �X�t�B�A�R���C�_�[�f�[�^�ǂݍ���
	LoadSphereColliders(pTextFile);

	return S_OK;
}

//==========================================================================
// �I�u�W�F�N�g���̃f�[�^���蓖��
//==========================================================================
void CObjectChara::BindObjectData(int nCntData)
{

	// �I�u�W�F�N�g���̃f�[�^���蓖��
	CObjectHierarchy::BindObjectData(nCntData);

	// �g��
	m_fHeight = m_aLoadData[nCntData].fHeight;

	// �ړ����x
	m_fVelocity = m_aLoadData[nCntData].fVelocity;

	// �̗�
	m_nLife = m_aLoadData[nCntData].nLife;
	m_nLifeOrigin = m_nLife;

	// �X�R�A���Z��
	m_nAddScore = m_aLoadData[nCntData].nAddScore;

	// ���[�V�����X�^�[�g�̃C���f�b�N�X
	m_nMotionStartIdx = m_aLoadData[nCntData].nMotionStartIdx;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CObjectChara::Init()
{
	// ����������
	CObjectHierarchy::Init();

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CObjectChara::Uninit()
{
	if (m_pMotion != nullptr)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = nullptr;
	}

	// �I������
	CObjectHierarchy::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CObjectChara::Update()
{
	// �I�u�W�F�N�g�q�G�����L�[�̍X�V
	CObjectHierarchy::Update();

	// ���[�V�����X�V
	if (m_pMotion != nullptr)
	{
		m_pMotion->Update();
	}

	// �R���C�_�[�̈ʒu����
	for (auto& collider : m_SphereColliders)
	{
		// ���肷��p�[�c�擾
		CModel* pModel = GetModel()[collider.nParentPartsIdx];

		// ���肷��p�[�c�̃}�g���b�N�X�擾
		D3DXMATRIX mtxTrans;
		D3DXMATRIX mtxWepon = pModel->GetWorldMtx();

		// �ʒu�𔽉f����
		D3DXMatrixTranslation(&mtxTrans, collider.offset.x, collider.offset.y, collider.offset.z);
		D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);
		collider.center = UtilFunc::Transformation::WorldMtxChangeToPosition(mtxWepon);
	}

	// ���[�V�������̍s������
	MotionInProgressAction();
}

//==========================================================================
// ���[�V�������̍s������
//==========================================================================
void CObjectChara::MotionInProgressAction()
{
	// �U������t���O���Z�b�g
	m_bInDicision = false;

	if (m_pMotion == nullptr)
	{
		return;
	}

	// ���擾
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());
	int nType = m_pMotion->GetType();

	// �U�����̑����擾
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (aInfo.AttackInfo[nCntAttack] == nullptr)
		{
			continue;
		}

		// �U�����擾
		CMotion::AttackInfo* AttackInfo = aInfo.AttackInfo[nCntAttack];

		if (m_pMotion->IsImpactFrame(*AttackInfo))
		{// �Ռ��̃J�E���g�Ɠ����Ƃ�

			// �U��������
			AttackAction(*AttackInfo, nCntAttack);
		}

		// ���[�V�����J�E���^�[�擾
		float fAllCount = m_pMotion->GetAllCount();
		if (fAllCount > AttackInfo->nMinCnt && fAllCount <= AttackInfo->nMaxCnt)
		{// �U�����蒆

			// �U������t���O
			m_bInDicision = true;

			// �U�����蒆����
			AttackInDicision(AttackInfo, nCntAttack);
		}
	}
}

//==========================================================================
// �U��������
//==========================================================================
void CObjectChara::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// ����̈ʒu
	MyLib::Vector3 weponpos = m_pMotion->GetAttackPosition(GetModel(), ATKInfo);
}

//==========================================================================
// �U�����蒆����
//==========================================================================
void CObjectChara::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
{
	// ����̈ʒu
	MyLib::Vector3 weponpos = m_pMotion->GetAttackPosition(GetModel(), *pATKInfo);
}

//==========================================================================
// ���[�V�����t�@�C���ύX
//==========================================================================
void CObjectChara::ChangeMotion(const char* pMotionFile)
{
	if (m_pMotion != nullptr)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = nullptr;
	}

	// ���[�V�����̐�������
	m_pMotion = CMotion::Create(pMotionFile, this);

	// ���[�V�����̐ݒ�
	m_pMotion->SetModel(GetModel(), GetNumModel());

	// �|�[�Y�̃��Z�b�g
	m_pMotion->ResetPose(0);
}

//==========================================================================
// �`�揈��
//==========================================================================
void CObjectChara::Draw()
{
	// �`�揈��
	CObjectHierarchy::Draw();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CObjectChara::Draw(D3DXCOLOR col)
{
	// �`�揈��
	CObjectHierarchy::Draw(col);
}

//==========================================================================
// �`�揈��
//==========================================================================
void CObjectChara::Draw(float fAlpha)
{
	// �`�揈��
	CObjectHierarchy::Draw(fAlpha);
}

//==========================================================================
// �I�u�W�F�N�g���̃f�[�^�ǂݍ���
//==========================================================================
void CObjectChara::LoadObjectData(FILE* pFile, const char* pComment)
{
	// �I�u�W�F�N�g���̃f�[�^�ǂݍ���
	CObjectHierarchy::LoadObjectData(pFile, pComment);

	char hoge[MAX_COMMENT];	// �R�����g

	if (strcmp(pComment, "HEIGHT") == 0)
	{// HEIGHT�Őg��

		fscanf(pFile, "%s", &hoge[0]);	// =�̕�
		fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad].fHeight);	// �g��
		m_fHeight = m_aLoadData[m_nNumLoad].fHeight;
	}

	if (strcmp(pComment, "VELOCITY") == 0)
	{// VELOCITY�ňړ����x

		fscanf(pFile, "%s", &hoge[0]);	// =�̕�
		fscanf(pFile, "%f", &m_aLoadData[m_nNumLoad].fVelocity);	// �ړ����x
		m_fVelocity = m_aLoadData[m_nNumLoad].fVelocity;
	}

	if (strcmp(pComment, "LIFE") == 0)
	{// LIFE�ő̗�

		fscanf(pFile, "%s", &hoge[0]);	// =�̕�
		fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad].nLife);	// �̗�
		m_nLife = m_aLoadData[m_nNumLoad].nLife;
		m_nLifeOrigin = m_nLife;	// ���̗̑�
	}

	if (strcmp(pComment, "MOTION_STARTPARTS") == 0)
	{// MOTION_STARTPARTS�Ń��[�V�����J�n�̃C���f�b�N�X�ԍ�

		fscanf(pFile, "%s", &hoge[0]);	// =�̕�
		fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad].nMotionStartIdx);	// ���[�V�����J�n�̃C���f�b�N�X�ԍ�
		m_nMotionStartIdx = m_aLoadData[m_nNumLoad].nMotionStartIdx;
	}

	if (strcmp(pComment, "SCORE") == 0)
	{// LIFE�ő̗�

		fscanf(pFile, "%s", &hoge[0]);	// =�̕�
		fscanf(pFile, "%d", &m_aLoadData[m_nNumLoad].nAddScore);	// �̗�
		m_nAddScore = m_aLoadData[m_nNumLoad].nAddScore;
	}

}

//==========================================================================
// �p�[�c���̃f�[�^�ǂݍ���
//==========================================================================
void CObjectChara::LoadPartsData(FILE* pFile, const char* pComment, int *pCntParts)
{
	// �p�[�c���̃f�[�^�ǂݍ���
	CObjectHierarchy::LoadPartsData(pFile, pComment, pCntParts);
}

//==========================================================================
// �X�t�B�A�R���C�_�[�ǂݍ���
//==========================================================================
void CObjectChara::LoadSphereColliders(const std::string textfile)
{
	// �t�@�C�����J��
	FILE* pFile = fopen(textfile.c_str(), "r");
	if (pFile == nullptr)
	{//�t�@�C�����J���Ȃ������ꍇ
		return;
	}

	std::string filename;

	// �ǂݍ��ݗp�ϐ�
	char aComment[MAX_COMMENT];	// �R�����g�p

	while (1)
	{
		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		// �R���C�_�[�t�@�C���̓ǂݍ���
		if (strcmp(aComment, "COLLIDER_FILENAME") == 0)
		{// COLLIDER_FILENAME��������

			fscanf(pFile, "%s", &aComment[0]);	// =�̕�
			fscanf(pFile, "%s", &aComment[0]);	// �t�@�C����

			// �t�@�C�����ۑ�
			filename = aComment;
			break;
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���
			break;
		}
	}

	// �t�@�C�������
	fclose(pFile);


	// �t�@�C������JSON��ǂݍ���
	std::ifstream file(filename);
	if (!file.is_open()) 
	{
		return;
	}

	nlohmann::json jsonData;
	file >> jsonData;	// json�f�[�^��^����

	// json�f�[�^����ǂݍ���
	from_json(jsonData);
}

//==========================================================================
// �X�t�B�A�R���C�_�[��������
//==========================================================================
void CObjectChara::SaveSphereColliders()
{
	// �t�@�C���ɃL�����N�^�[�̃f�[�^����������
	std::ofstream outFile("character_data.json");
	if (!outFile.is_open())
	{
		return;
	}

	nlohmann::json outputData;
	to_json(outputData);
}

//==========================================================================
// �X�t�B�A�R���C�_�[�̐��擾
//==========================================================================
int CObjectChara::GetSphereColliderNum()
{
	return static_cast<int>(m_SphereColliders.size());
}

//==========================================================================
// �X�t�B�A�R���C�_�[�擾
//==========================================================================
std::vector<CObjectChara::SphereCollider> CObjectChara::GetSphereColliders()
{ 
	return m_SphereColliders; 
}

//==========================================================================
// �R���C�_�[�擾
//==========================================================================
CObjectChara::SphereCollider CObjectChara::GetNowSphereCollider(int nIdx)
{
	if (nIdx >= 0 && nIdx < static_cast<int>(m_SphereColliders.size()))
	{
#if 0
		// ���肷��p�[�c�擾
		CModel* pModel = GetModel()[m_SphereColliders[nIdx].nParentPartsIdx];

		// ���肷��p�[�c�̃}�g���b�N�X�擾
		D3DXMATRIX mtxTrans;
		D3DXMATRIX mtxWepon = pModel->GetWorldMtx();

		// �ʒu�𔽉f����
		D3DXMatrixTranslation(&mtxTrans, m_SphereColliders[nIdx].offset.x, m_SphereColliders[nIdx].offset.y, m_SphereColliders[nIdx].offset.z);
		D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);
		m_SphereColliders[nIdx].center = UtilFunc::Transformation::WorldMtxChangeToPosition(mtxWepon);
#endif
		return m_SphereColliders[nIdx];
	}
	return SphereCollider();
}

//==========================================================================
// �g���ݒ�
//==========================================================================
void CObjectChara::SetHeight(const float fHeight)
{
	m_fHeight = fHeight;
}

//==========================================================================
// �g���擾
//==========================================================================
float CObjectChara::GetHeight()
{
	return m_fHeight;
}

//==========================================================================
// �ړ����x�ݒ�
//==========================================================================
void CObjectChara::SetVelocity(const float fVelocity)
{
	m_fVelocity = fVelocity;
}

//==========================================================================
// �ړ����x�ݒ�
//==========================================================================
float CObjectChara::GetVelocity() const
{
	return m_fVelocity;
}

//==========================================================================
// �ڕW�̌����ݒ�
//==========================================================================
void CObjectChara::SetRotDest(const float fRotDest)
{
	m_fRotDest = fRotDest;
}

//==========================================================================
// �ڕW�̌����ݒ�
//==========================================================================
float CObjectChara::GetRotDest() const
{
	return m_fRotDest;
}

//==========================================================================
// �̗͐ݒ�
//==========================================================================
void CObjectChara::SetLife(const int nLife)
{
	m_nLife = nLife;
}

//==========================================================================
// �̗͎擾
//==========================================================================
int CObjectChara::GetLife() const
{
	return m_nLife;
}

//==========================================================================
// ���̗̑͐ݒ�
//==========================================================================
void CObjectChara::SetLifeOrigin(const int nLife)
{
	m_nLifeOrigin = nLife;
}

//==========================================================================
// ���̗͎̑擾
//==========================================================================
int CObjectChara::GetLifeOrigin() const
{
	return m_nLifeOrigin;
}

//==========================================================================
// ���[�V�����J�n�̃C���f�b�N�X�ԍ��擾
//==========================================================================
int CObjectChara::GetMotionStartIdx() const
{
	return m_nMotionStartIdx;
}

//==========================================================================
// �X�R�A���Z�ʎ擾
//==========================================================================
int CObjectChara::GetAddScoreValue() const
{
	return m_nAddScore;
}

//==========================================================================
// ���[�V�����I�u�W�F�N�g�擾
//==========================================================================
CMotion* CObjectChara::GetMotion()
{
	return m_pMotion;
}

//==========================================================================
// �I�u�W�F�N�g�L�����N�^�[�I�u�W�F�N�g�̎擾
//==========================================================================
CObjectChara* CObjectChara::GetObjectChara()
{
	return this;
}
