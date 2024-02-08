//=============================================================================
// 
//  �ϏO���� [gallery.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "gallery.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "calculation.h"
#include "objectX.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* LOADTEXT = "data\\TEXT\\map\\gallery.txt";
	const char* FILENAME[3][3] =
	{
		{
			"data\\MODEL\\gallery\\gallery_red_wait.x",
			"data\\MODEL\\gallery\\gallery_red_move01.x",
			"data\\MODEL\\gallery\\gallery_red_move02.x",
		},
		{
			"data\\MODEL\\gallery\\gallery_blue_wait.x",
			"data\\MODEL\\gallery\\gallery_blue_move01.x",
			"data\\MODEL\\gallery\\gallery_blue_move02.x",
		},
		{
			"data\\MODEL\\gallery\\gallery_green_wait.x",
			"data\\MODEL\\gallery\\gallery_green_move01.x",
			"data\\MODEL\\gallery\\gallery_green_move02.x",
		},
	};
	const int NUM_GALLERY = 7;	// �ϏO�̐l��
	const float DISTANCE = 80.0f;	// �Ԋu
	const float DISTANCE_DEPTH = 70.0f;	// ���s���̊Ԋu
	const float GRAVITY = 0.5f;		// �d��
	const float JUMP = 6.0f;		// �W�����v��
	const float TIME_COUNTERHEAT = 1.8f;	// �J�E���^�[����オ��̎���
}
std::vector<int> CGallery::m_nModelIdx = {};		// ���f���C���f�b�N�X�ԍ�
CListManager<CGallery> CGallery::m_List = {};		// ���X�g

//==========================================================================
// �֐��|�C���^
//==========================================================================
CGallery::STATE_FUNC CGallery::m_StateFunc[] =
{
	&CGallery::StateNone,	// �Ȃ�
	&CGallery::StateClearHeat,	// �N���A����オ��
	&CGallery::StateCounterHeat,	// �J�E���^�[����オ��
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CGallery::CGallery(int nPriority) : CObject(nPriority)
{
	// �N���A
	m_State = STATE_NONE;	// ���
	m_fStateTime = 0.0f;	// ��ԃJ�E���^�[
	m_GalleryInfo.clear();	// �ϏO���
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CGallery::~CGallery()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CGallery* CGallery::Create(const MyLib::Vector3& pos, const MyLib::Vector3& rot)
{
	// �����p�̃I�u�W�F�N�g
	CGallery* pGallery = nullptr;

	// �������̊m��
	pGallery = DEBUG_NEW CGallery;

	if (pGallery != nullptr)
	{
		// ���ݒ�
		pGallery->SetOriginPosition(pos);
		pGallery->SetPosition(pos);
		pGallery->SetRotation(rot);

		// ����������
		if (FAILED(pGallery->Init()))
		{
			return nullptr;
		}
	}

	return pGallery;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CGallery::Init()
{
	CXLoad* pXload = CXLoad::GetInstance();

	// ���f���C���f�b�N�X�ԍ�
	if (m_nModelIdx.empty())
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				m_nModelIdx.emplace_back();
				m_nModelIdx.back() = pXload->XLoad(FILENAME[i][j]);
			}
		}
	}

	// ��ނ̐ݒ�
	SetType(TYPE_OBJECTX);

	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();

	for (int height = 0; height < 3; height++)
	{
		for (int i = 0; i < NUM_GALLERY; i++)
		{
			// �������̈ʒu
			MyLib::Vector3 spawnpos = pos;
			spawnpos.y += height * 40.0f;

			spawnpos.x = (pos.x + sinf(D3DX_PI * 0.5f + rot.y) * (i * DISTANCE)) - sinf(D3DX_PI * 0.5f + rot.y) * (DISTANCE * (NUM_GALLERY / 2));
			spawnpos.z = (pos.z + cosf(D3DX_PI * 0.5f + rot.y) * (i * DISTANCE)) - cosf(D3DX_PI * 0.5f + rot.y) * (DISTANCE * (NUM_GALLERY / 2));
			spawnpos.x += UtilFunc::Transformation::Random(-20, 20);
			spawnpos.z += UtilFunc::Transformation::Random(-20, 20);

			// �i�̕����炷
			spawnpos.x -= sinf(D3DX_PI + rot.y) * (height * DISTANCE_DEPTH);
			spawnpos.z -= cosf(D3DX_PI + rot.y) * (height * DISTANCE_DEPTH);

			// �������̌���
			MyLib::Vector3 spawnrot = rot;
			spawnrot.y += D3DXToRadian(UtilFunc::Transformation::Random(-10, 10));


			// �ϏO����
			m_GalleryInfo.emplace_back();
			m_GalleryInfo.back().colorType = static_cast<COLORTYPE>(UtilFunc::Transformation::Random(0, static_cast<int>(COLORTYPE_MAX - 1)));	// �F�̎��
			m_GalleryInfo.back().objX = CObjectX::Create(FILENAME[m_GalleryInfo.back().colorType][0], spawnpos, spawnrot, false);

			if (m_GalleryInfo.back().objX == nullptr)
			{
				return E_FAIL;
			}
			m_GalleryInfo.back().objX->SetOriginPosition(spawnpos);

			// ��ސݒ�
			m_GalleryInfo.back().objX->SetType(CObject::TYPE_OBJECTX);
		}
	}

	// ���X�g�Ɋ��蓖��
	m_List.Regist(this);
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CGallery::Uninit()
{
	for (const auto& info : m_GalleryInfo)
	{
		info.objX->Uninit();
	}

	// �N���A
	m_GalleryInfo.clear();

	// ���X�g����폜
	m_List.Delete(this);

	// ���폜
	Release();

	// �C���f�b�N�X���Z�b�g
	m_nModelIdx.clear();
}

//==========================================================================
// �폜����
//==========================================================================
void CGallery::Kill()
{
	for (const auto& info : m_GalleryInfo)
	{
		info.objX->Kill();
	}

	// �N���A
	m_GalleryInfo.clear();

	// ���X�g����폜
	m_List.Delete(this);

	// ���폜
	Release();
}


//==========================================================================
// �X�V����
//==========================================================================
void CGallery::Update()
{
	// ��ԍX�V
	(this->*(m_StateFunc[m_State]))();
}

//==========================================================================
// �N���A����オ��
//==========================================================================
void CGallery::StateClearHeat()
{
	// �W�����v
	Jump();
}

//==========================================================================
// �J�E���^�[����オ��
//==========================================================================
void CGallery::StateCounterHeat()
{

	// ��ԃJ�E���^�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	if (TIME_COUNTERHEAT <= m_fStateTime)
	{
		for (auto& info : m_GalleryInfo)
		{
			MyLib::Vector3 move = info.objX->GetMove();
			MyLib::Vector3 pos = info.objX->GetPosition();
			MyLib::Vector3 posorigin = info.objX->GetOriginPosition();

			// �ʒu�X�V
			pos += move;

			// �d��
			move.y -= GRAVITY;
			info.objX->SetMove(move);

			if (pos.y <= posorigin.y)
			{// �����l��������

				// �����l�Œ�
				pos.y = posorigin.y;
				info.objX->SetMove(0.0f);

				// �ʏ�̃|�[�Y�ɖ߂�
				info.poseType = POSE_NONE;

				// ���f���؂�ւ�
				int color = info.colorType, pose = info.poseType;
				info.objX->BindXData(m_nModelIdx[3 * color + pose]);
			}

			info.objX->SetPosition(pos);
		}
	}
	else
	{
		// �W�����v
		Jump();
	}
}

//==========================================================================
// �W�����v
//==========================================================================
void CGallery::Jump()
{
	for (auto& info : m_GalleryInfo)
	{
		MyLib::Vector3 move = info.objX->GetMove();
		MyLib::Vector3 pos = info.objX->GetPosition();
		MyLib::Vector3 posorigin = info.objX->GetOriginPosition();

		// �ʒu�X�V
		pos += move;

		// �d��
		move.y -= GRAVITY;
		info.objX->SetMove(move);

		if (pos.y <= posorigin.y)
		{// �����l��������

			pos.y = posorigin.y;

			float jump = JUMP + UtilFunc::Transformation::Random(-20, 20) * 0.05f;
			info.objX->SetMove(MyLib::Vector3(0.0f, jump, 0.0f));

			info.poseType =
				(info.poseType == POSE_RIGHT) ? POSE_LEFT :
				(info.poseType == POSE_LEFT) ? POSE_RIGHT :
				static_cast<POSE>(UtilFunc::Transformation::Random(1, 2));

			// ���f���؂�ւ�
			int color = info.colorType, pose = info.poseType;
			info.objX->BindXData(m_nModelIdx[3 * color + pose]);
		}

		info.objX->SetPosition(pos);
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CGallery::Draw()
{
	

}

//==========================================================================
// ��Ԑݒ�
//==========================================================================
void CGallery::SetState(STATE state)
{ 
	m_fStateTime = 0.0f;
	m_State = state;
}

//==========================================================================
// �ϏO�ݒ�
//==========================================================================
void CGallery::SetGallery(void)
{
	char aComment[MAX_COMMENT] = {};	// �R�����g�p
	int nTexNum = 0;					// �t�@�C���̐�
	int nCntTexture = 0;				// �e�N�X�`���ǂݍ��݃J�E���g

	//�t�@�C�����J��
	FILE* pFile = fopen(LOADTEXT, "r");

	if (pFile == nullptr)
	{//�t�@�C�����J�����ꍇ
		return;
	}

	while (1)
	{// END_SCRIPT������܂ŌJ��Ԃ�

		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		// ���f���̐ݒ�
		if (strcmp(&aComment[0], "MODELSET") == 0)
		{// ���f���̓ǂݍ��݂��J�n

			MyLib::Vector3 pos = mylib_const::DEFAULT_VECTOR3;
			MyLib::Vector3 rot = mylib_const::DEFAULT_VECTOR3;

			while (strcmp(&aComment[0], "END_MODELSET"))
			{// END_MODELSET������܂ŌJ��Ԃ�

				fscanf(pFile, "%s", &aComment[0]);	// �m�F����

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

			// ����
			CGallery::Create(pos, rot);
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���

			break;
		}
	}

	// �t�@�C�������
	fclose(pFile);
}
