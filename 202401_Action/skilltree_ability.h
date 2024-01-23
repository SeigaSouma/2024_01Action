//=============================================================================
// 
// �X�L���c���[�\�͏��� [skilltree_ability.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_ABILITY_H_
#define _SKILLTREE_ABILITY_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[�\��
class CSkillTree_Ability
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	// �J�e�S���[
	enum eSkillCategory
	{
		SKILLCATEGORY_BUFF = 0,	// �o�t
		SKILLCATEGORY_UNLOCK,	// �A�����b�N
		SKILLCATEGORY_MAX
	};


	//=============================
	// �\���̒�`
	//=============================
	// �X�L���̃X�e�[�W
	struct sSkillStage
	{
		int stage;	// �i�K

		// �f�t�H���g�R���X�g���N�^
		sSkillStage() : stage(0) {}

		// �p�����[�^���R���X�g���N�^
		sSkillStage(int stage)
			: stage(stage) {}

		
		// JSON����̓ǂݍ���
		void from_json(const json& j)
		{
			j.at("stage").get_to(stage);	// �i�K
		}

		// JSON�ւ̏�������
		void to_json(json& j) const
		{
			j = json
			{
				{"stage", stage},	// �i�K
			};
		}
	};

	// �X�L���̎��
	struct sSkillType
	{
		std::string typeName;	// ��ނ̖��O
		std::vector<sSkillStage> stage;	// �X�L���̃X�e�[�W

		// �f�t�H���g�R���X�g���N�^
		sSkillType() {}

		// �p�����[�^���R���X�g���N�^
		sSkillType(const std::string& name, const std::vector<sSkillStage>& stagelist)
			: typeName(name), stage(stagelist) {}

		
		// JSON����̓ǂݍ���
		void from_json(const json& j)
		{
			j.at("typeName").get_to(typeName);	// ��ނ̖��O

			// �X�L���̃X�e�[�W
			for (const auto& type : j.at("skillstage"))
			{
				sSkillStage loacalstage;
				loacalstage.from_json(type);
				stage.push_back(loacalstage);
			}
		}

		// JSON�ւ̏�������
		void to_json(json& j) const
		{
			j = json
			{
				{"typeName", typeName},	// ��ނ̖��O
			};

			j["skillstage"] = json::array(); // ��̔z����쐬

			// �X�L���̃X�e�[�W
			for (const auto& type : stage)
			{
				json jsonData;
				type.to_json(jsonData);

				j["skillstage"].emplace_back(jsonData);
			}
		}

	};

	// �X�L���̃J�e�S���[
	struct sSkillCategory
	{
		eSkillCategory categoryType;;	// �J�e�S���[�̗�
		std::vector<sSkillType> type;	// �X�L���̎��

		// �f�t�H���g�R���X�g���N�^
		sSkillCategory() : categoryType(SKILLCATEGORY_BUFF) {}

		// �p�����[�^���R���X�g���N�^
		sSkillCategory(eSkillCategory type, const std::vector<sSkillType>& typelist)
			: categoryType(type), type(typelist) {}


		// JSON����̓ǂݍ���
		void from_json(const json& j)
		{
			j.at("categorytype").get_to(categoryType);	// �J�e�S���[�̗�

			// �X�L���̎��
			for (const auto& category : j.at("skilltypes"))
			{
				sSkillType loacaltype;
				loacaltype.from_json(category);
				type.push_back(loacaltype);
			}
		}

		// JSON�ւ̏�������
		void to_json(json& j) const
		{
			j = json
			{
				{"categorytype", categoryType},	// �J�e�S���[�̗�
			};

			j["skilltypes"] = json::array(); // ��̔z����쐬

			// �X�L���̎��
			for (const auto& category : type)
			{
				json jsonData;
				category.to_json(jsonData);

				j["skilltypes"].emplace_back(jsonData);
			}
		}
	};

	// JSON����̓ǂݍ���
	void from_json(const json& j)
	{
		// �X�L���̃J�e�S���[
		for (const auto& ability : j.at("skillcategorys"))
		{
			sSkillCategory loacalcategory;
			loacalcategory.from_json(ability);
			m_SkillCategory.push_back(loacalcategory);
		}
	}

	// JSON�ւ̏�������
	void to_json(json& j) const
	{

		j["skillcategorys"] = json::array(); // ��̔z����쐬

		// �X�L���̃J�e�S���[
		for (const auto& ability : m_SkillCategory)
		{
			json jsonData;
			ability.to_json(jsonData);

			j["skillcategorys"].emplace_back(jsonData);
		}
	}
	

	CSkillTree_Ability();
	~CSkillTree_Ability();
	
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void LoadJson(void);	// Json����̃��[�h
	void SaveJson(void);	// Json�ւ̃Z�[�u

	static CSkillTree_Ability* GetInstance() { return m_pThisPtr; }	// �C���X�^���X�擾
	static CSkillTree_Ability* Create(void);	// ��������

private:

	//=============================
	// �����o�ϐ�
	//=============================
	std::vector<sSkillCategory> m_SkillCategory;
	static CSkillTree_Ability* m_pThisPtr;	// ���g�̃|�C���^
};

#endif