//=============================================================================
// 
//  �������� [torch.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "torch.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"
#include "calculation.h"
#include "game.h"
#include "particle.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* LOADTEXT = "data\\TEXT\\map\\torch.txt";
	const char* MODEL_STAND = "data\\MODEL\\arena\\taimatu_002.x";
	const char* MODEL_WALL = "data\\MODEL\\arena\\taimatu_001.x";
}
std::vector<std::string> CTorch::ModelFile = {};		// ���f���t�@�C����
CListManager<CTorch> CTorch::m_List = {};		// ���X�g

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTorch::CTorch(int nPriority) : CObjectX(nPriority)
{
	// �l�̃N���A
	m_nCntFire = 0;
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTorch::~CTorch()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CTorch *CTorch::Create(TYPE type, const MyLib::Vector3& pos, const MyLib::Vector3& rot)
{

	CTorch *pTorch = nullptr;

	// �������m��
	switch (type)
	{
	case CTorch::TYPE_STAND:
		pTorch = DEBUG_NEW CTorchStand;
		break;

	case CTorch::TYPE_WALL:
		pTorch = DEBUG_NEW CTorchWall;
		break;

	default:
		return nullptr;
		break;
	}

	if (pTorch != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		pTorch->SetPosition(pos);
		pTorch->SetRotation(rot);

		// ����������
		pTorch->Init();
	}

	return pTorch;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTorch::Init()
{
	// ��ނ̐ݒ�
	CObject::SetType(TYPE_OBJECTX);

	// ���X�g�Ɋ��蓖��
	m_List.Regist(this);

	return S_OK;
}

//==========================================================================
// �u���^����������
//==========================================================================
HRESULT CTorchStand::Init()
{
	HRESULT hr;

	// ����������
	if (FAILED(CTorch::Init()))
	{
		return E_FAIL;
	}

	hr = CObjectX::Init(MODEL_STAND);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	return S_OK;
}

//==========================================================================
// �u���^����������
//==========================================================================
HRESULT CTorchWall::Init()
{
	HRESULT hr;

	// ����������
	if (FAILED(CTorch::Init()))
	{
		return E_FAIL;
	}

	hr = CObjectX::Init(MODEL_WALL);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CTorch::Uninit()
{
	// ���X�g����폜
	m_List.Delete(this);

	// �I������
	CObjectX::Uninit();
}

//==========================================================================
// �폜
//==========================================================================
void CTorch::Kill()
{
	// ���X�g����폜
	m_List.Delete(this);

	// �폜����
	CObjectX::Kill();
}

//==========================================================================
// �u���^�X�V����
//==========================================================================
void CTorchStand::Update()
{
	m_nCntFire = (m_nCntFire + 1) % 10;

	if (m_nCntFire == 0)
	{
		my_particle::Create(GetPosition(), my_particle::TYPE_FIRE_STANDTORCH);
	}
}

//==========================================================================
// �Ǌ|���X�V����
//==========================================================================
void CTorchWall::Update()
{
	m_nCntFire = (m_nCntFire + 1) % 10;

	if (m_nCntFire == 0)
	{
		my_particle::Create(GetPosition(), my_particle::TYPE_FIRE_WALLTORCH);
	}
}

//==========================================================================
// ���f���ǂݍ��ݏ���
//==========================================================================
HRESULT CTorch::ReadXFile(const char* pTextFile)
{
	if (!ModelFile.empty())
	{
		return S_OK;
	}

	// ���Z�b�g
	int nNumFileAll = 0;

	char aComment[MAX_COMMENT] = {};	// �R�����g�p
	int nFileNum = 0;					// �t�@�C���̐�
	int nCntTexture = 0;				// �e�N�X�`���ǂݍ��݃J�E���g

	//�t�@�C�����J��
	FILE* pFile = fopen(pTextFile, "r");

	if (pFile == nullptr)
	{//�t�@�C�����J�����ꍇ
		return E_FAIL;
	}

	while (1)
	{// END_SCRIPT������܂ŌJ��Ԃ�

		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		// ���f�����̐ݒ�
		if (strcmp(&aComment[0], "NUM_MODEL") == 0)
		{// NUM_MODEL��������

			fscanf(pFile, "%s", &aComment[0]);	// =�̕�
			fscanf(pFile, "%d", &nFileNum);	// ���f����
		}

		while (nNumFileAll != nFileNum)
		{// ���f���̐����ǂݍ��ނ܂ŌJ��Ԃ�

			// ������̓ǂݍ���
			fscanf(pFile, "%s", &aComment[0]);

			// ���f�����̐ݒ�
			if (strcmp(&aComment[0], "MODEL_FILENAME") == 0)
			{// NUM_MODEL��������

				fscanf(pFile, "%s", &aComment[0]);	// =�̕�
				fscanf(pFile, "%s", &aComment[0]);	// �t�@�C����

				// �Ō���ɒǉ�
				ModelFile.push_back(&aComment[0]);

				// �t�@�C����
				nNumFileAll++;
			}
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���

			break;
		}
	}

	// �t�@�C�������
	fclose(pFile);

	return S_OK;
}

//==========================================================================
// �O���e�L�X�g�ǂݍ��ݏ���
//==========================================================================
void CTorch::SetTorch()
{
	// ���f���ǂݍ���
	if (FAILED(ReadXFile(LOADTEXT)))
	{// ���s�����ꍇ
		return;
	}

	char aComment[MAX_COMMENT] = {};	//�R�����g�p

	// �t�@�C���|�C���^
	FILE* pFile = nullptr;

	//�t�@�C�����J��
	pFile = fopen(LOADTEXT, "r");

	if (pFile == nullptr)
	{
		return;
	}

	while (1)
	{// END_SCRIPT������܂ŌJ��Ԃ�

		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		// ���f���̐ݒ�
		if (strcmp(&aComment[0], "MODELSET") == 0)
		{// ���f���̓ǂݍ��݂��J�n

			int nType = 0;
			MyLib::Vector3 pos = mylib_const::DEFAULT_VECTOR3;
			MyLib::Vector3 rot = mylib_const::DEFAULT_VECTOR3;

			while (strcmp(&aComment[0], "END_MODELSET"))
			{// END_MODELSET������܂ŌJ��Ԃ�

				fscanf(pFile, "%s", &aComment[0]);	// �m�F����

				if (strcmp(&aComment[0], "TYPE") == 0)
				{// TYPE���������ޓǂݍ���

					fscanf(pFile, "%s", &aComment[0]);	// =�̕�
					fscanf(pFile, "%d", &nType);		// ���f����ނ̗�
				}

				if (strcmp(&aComment[0], "POS") == 0)
				{// POS��������ʒu�ǂݍ���

					fscanf(pFile, "%s", &aComment[0]);	// =�̕�
					fscanf(pFile, "%f", &pos.x);		// X���W
					fscanf(pFile, "%f", &pos.y);		// Y���W
					fscanf(pFile, "%f", &pos.z);		// Z���W
				}

				if (strcmp(&aComment[0], "ROT") == 0)
				{// ROT������������ǂݍ���

					fscanf(pFile, "%s", &aComment[0]);	// =�̕�
					fscanf(pFile, "%f", &rot.x);		// X�̌���
					fscanf(pFile, "%f", &rot.y);		// Y�̌���
					fscanf(pFile, "%f", &rot.z);		// Z�̌���
				}
			}// END_MODELSET�̂�����
			
			// �^�C�v�̕��𐶐�
			CTorch::Create(static_cast<TYPE>(nType), pos, rot);
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���

			break;
		}
	}

	// �t�@�C�������
	fclose(pFile);
}
