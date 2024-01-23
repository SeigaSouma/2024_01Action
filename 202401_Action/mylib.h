//=============================================================================
// 
// ���g�̃��C�u���� [mylib.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _MYLIB_H_
#define _MYLIB_H_		// ��d�C���N���[�h�h�~

#include <json.hpp>

/**
	@brief	�}�C���C�u����
*/
namespace MyLib
{

	/**
	@brief	Vector3 �y3�������z
	*/
	struct Vector3 : public D3DXVECTOR3
	{
		using D3DXVECTOR3::D3DXVECTOR3;

		// �f�t�H���g�R���X�g���N�^
		Vector3() : D3DXVECTOR3(0.0f, 0.0f, 0.0f) {}

		// �����t���R���X�g���N�^
		Vector3(const float xyz) : D3DXVECTOR3(xyz, xyz, xyz) {}
		Vector3(float x, float y, float z) : D3DXVECTOR3(x, y, z) {}

		// JSON����̓ǂݍ���
		void from_json(const nlohmann::json& j)
		{
			j.at("x").get_to(x);
			j.at("y").get_to(y);
			j.at("z").get_to(z);
		}

		// JSON�ւ̏�������
		void to_json(nlohmann::json& j) const
		{
			j["x"] = x;
			j["y"] = y;
			j["z"] = z;
		}

		//--------------------------
		// ���Z
		//--------------------------
		inline Vector3 operator + (const float& o) const
		{
			return Vector3(x + o, y + o, z + o);
		}

		inline Vector3 operator + (const Vector3& o) const
		{
			return Vector3(x + o.x, y + o.y, z + o.z);
		}

		//--------------------------
		// ���Z
		//--------------------------
		inline Vector3 operator - (const float& o) const
		{
			return Vector3(x - o, y - o, z - o);
		}

		inline Vector3 operator - () const
		{
			return Vector3(-x, -y, -z);
		}

		inline Vector3 operator - (const Vector3& o) const
		{
			return Vector3(x - o.x, y - o.y, z - o.z);
		}

		//--------------------------
		// ���Z���
		//--------------------------
		inline Vector3& operator += (const float& o)
		{
			x += o;
			y += o;
			z += o;
			return *this;
		}

		inline Vector3& operator += (const Vector3& o)
		{
			x += o.x;
			y += o.y;
			z += o.z;
			return *this;
		}

		//--------------------------
		// ���Z���
		//--------------------------
		inline Vector3& operator -= (const float& o)
		{
			x -= o;
			y -= o;
			z -= o;
			return *this;
		}

		inline Vector3& operator -= (const Vector3& o)
		{
			x -= o.x;
			y -= o.y;
			z -= o.z;
			return *this;
		}

		/**
		@brief	�x�N�g���̒���
		@return	���߂�ꂽ����
		*/
		inline float Length(void)
		{
			return sqrtf(x * x + y * y + z * z);
		}

		/**
		@brief	�x�N�g���̒�����2��
		@return	���߂�ꂽ����
		*/
		inline float LengthSquared(void) const
		{
			return x * x + y * y + z * z;
		}

		/**
		@brief	�x�N�g���̋���
		@return	2�̃x�N�g���̋���
		*/
		float Distance(const Vector3& v) const 
		{
			return sqrtf((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) + (v.z - z) * (v.z - z));
		}

		/**
		@brief	�x�N�g���̋�����2��
		@return	2�̃x�N�g���̋�����2��
		*/
		float DistSquared(const Vector3& v) const 
		{
			return (v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) + (v.z - z) * (v.z - z);
		}

		/**
		@brief	�P�ʃx�N�g��
		@return	���K�����ꂽ�x�N�g��
		*/
		inline Vector3 Normal(void)
		{
			Vector3 result;
			D3DXVec3Normalize(&result, this);
			return result;
		}

		/**
		@brief	�O��
		@return	2�̃x�N�g���̊O��
		*/
		Vector3 Cross(const Vector3& v) const 
		{
			return Vector3(
				y * v.z - z * v.y,
				z * v.x - x * v.z,
				x * v.y - y * v.x);
		}

		/**
		@brief	����
		@return	2�̃x�N�g���̓���
		*/
		Vector3 Dot(const Vector3& v) const
		{
			return Vector3(x * v.x + y * v.y + z * v.z);
		}

		/**
		@brief	���������ϊ�
		@return	2�_�̐��������p�x
		*/
		float GetHorizontalAngle(const Vector3& v) const 
		{
			return atan2f((x - v.x), (z - v.z));
		}

		/**
		@brief	�x�N�g���𔽓]
		@return	���]���ꂽ�x�N�g��
		*/
		inline Vector3 Invert(void)
		{
			return Vector3(-x, -y, -z);
		}

		/**
		@brief	�w�肳�ꂽ���e�덷�͈͓��ɂ��邩����
		@param	range	[in]	���e�͈�
		@return	���茋��
		*/
		inline bool IsNearlyZero(float range) const
		{
			if (abs(x) <= range &&
				abs(y) <= range &&
				abs(z) <= range)
			{
				return true;
			}
			return false;
		}

		/**
		@brief	�v�f���[����
		@return	���茋��
		*/
		inline bool IsZero(void) const
		{
			if (x == 0.0f &&
				y == 0.0f &&
				z == 0.0f)
			{
				return true;
			}
			return false;
		}

		/**
		@brief	������ϊ�
		@return	(x, y, z)�̌`���ŕϊ����ꂽ������
		*/
		inline std::string ToString(void) const
		{
			std::stringstream text;
			text << "(" << x << ", " << y << ", " << z << ")";
			return text.str();
		}
	};

	/**
	@brief	HitResult�y�Փˏ��z
	*/
	struct HitResult
	{
		bool ishit;				// �Փˌ���
		MyLib::Vector3 hitpos;	// �Փ˒n�_

		// �f�t�H���g�R���X�g���N�^
		HitResult() : ishit(false), hitpos() {}

		// �p�����[�^�t���R���X�g���N�^
		HitResult(bool bHit, MyLib::Vector3 pos) : ishit(bHit), hitpos(pos) {}
	};
}


// nlohmann::json�̓��ꉻ
namespace nlohmann 
{
	template <> struct adl_serializer<MyLib::Vector3>
	{
		static void to_json(json& j, const MyLib::Vector3& v)
		{
			v.to_json(j);
		}

		static void from_json(const json& j, MyLib::Vector3& v)
		{
			v.from_json(j);
		}
	};
}

#endif