//=============================================================================
// 
// �X�L���c���[�\�͏��� [skilltree_ability.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_ability.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "calculation.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* LOADTEXT = "data\\TEXT\\skilltree\\ability.json";
}

//==========================================================================
// �ÓI�����o�ϐ���`
//==========================================================================
CSkillTree_Ability* CSkillTree_Ability::m_pThisPtr = nullptr;	// ���g�̃|�C���^

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Ability::CSkillTree_Ability()
{

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_Ability::~CSkillTree_Ability()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Ability* CSkillTree_Ability::Create()
{

	if (m_pThisPtr == NULL)
	{// �܂��������Ă��Ȃ�������

		// �C���X�^���X����
		m_pThisPtr = DEBUG_NEW CSkillTree_Ability;

		// ����������
		m_pThisPtr->Init();
	}
	else
	{
		// �C���X�^���X�擾
		m_pThisPtr->GetInstance();
	}

	return m_pThisPtr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillTree_Ability::Init()
{
#if 1

	// Json����ǂݍ���
	LoadJson();

#else

	m_SkillCategory.push_back(sSkillCategory());

	// ���݈����J�e�S���[�̃|�C���^
	sSkillCategory* pCategory = &m_SkillCategory[SKILLCATEGORY_BUFF];

	// �񋓐ݒ�
	pCategory->categoryType = SKILLCATEGORY_BUFF;

	// ��ޒǉ�
	pCategory->type.push_back(sSkillType());
	pCategory->type.back().typeName = "Stamina";

	// �X�e�[�W�ǉ�
	pCategory->type.back().stage.push_back(sSkillStage());
	pCategory->type.back().stage.back().stage = 0;

	// �X�e�[�W�ǉ�
	pCategory->type.back().stage.push_back(sSkillStage());
	pCategory->type.back().stage.back().stage = 1;

	// Json�֏�������
	SaveJson();
#endif

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree_Ability::Uninit()
{
	if (m_pThisPtr != nullptr)
	{
		delete m_pThisPtr;
		m_pThisPtr = nullptr;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillTree_Ability::Update()
{
	for (const auto& category : m_SkillCategory)
	{
		category.categoryType;
		for (const auto& type : category.type)
		{
			type.typeName;
			for (const auto& stage : type.stage)
			{
				stage.stage;
			}
		}
	}
}

//==========================================================================
// Json����̃��[�h
//==========================================================================
void CSkillTree_Ability::LoadJson()
{
	// �t�@�C������JSON��ǂݍ���
	std::ifstream file(LOADTEXT);
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
// Json�ւ̃Z�[�u
//==========================================================================
void CSkillTree_Ability::SaveJson()
{
	// �t�@�C���ɃL�����N�^�[�̃f�[�^����������
	std::ofstream outFile(LOADTEXT);
	if (!outFile.is_open())
	{
		return;
	}

	// �o�̓f�[�^
	nlohmann::json outputData;
	to_json(outputData);

	// �t�@�C���Ƀf�[�^����������
	outFile << std::setw(4) << outputData << std::endl;
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSkillTree_Ability::Draw()
{

}
