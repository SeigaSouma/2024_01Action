//=============================================================================
// 
// 自身のライブラリ [mylib.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MYLIB_H_
#define _MYLIB_H_		// 二重インクルード防止

#include <json.hpp>

/**
	@brief	マイライブラリ
*/
namespace MyLib
{

	/**
	@brief	Vector3 【3次元情報】
	*/
	struct Vector3 : public D3DXVECTOR3
	{
		using D3DXVECTOR3::D3DXVECTOR3;

		// デフォルトコンストラクタ
		Vector3() : D3DXVECTOR3(0.0f, 0.0f, 0.0f) {}

		// 引数付きコンストラクタ
		Vector3(const float xyz) : D3DXVECTOR3(xyz, xyz, xyz) {}
		Vector3(float x, float y, float z) : D3DXVECTOR3(x, y, z) {}

		// JSONからの読み込み
		void from_json(const nlohmann::json& j)
		{
			j.at("x").get_to(x);
			j.at("y").get_to(y);
			j.at("z").get_to(z);
		}

		// JSONへの書き込み
		void to_json(nlohmann::json& j) const
		{
			j["x"] = x;
			j["y"] = y;
			j["z"] = z;
		}

		//--------------------------
		// 加算
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
		// 減算
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
		// 加算代入
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
		// 減算代入
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
		@brief	ベクトルの長さ
		@return	求められた長さ
		*/
		inline float Length()
		{
			return sqrtf(x * x + y * y + z * z);
		}

		/**
		@brief	XZベクトルの長さ
		@return	求められた長さ
		*/
		inline float LengthXZ()
		{
			return sqrtf(x * x + z * z);
		}

		/**
		@brief	XYベクトルの長さ
		@return	求められた長さ
		*/
		inline float LengthXY()
		{
			return sqrtf(x * x + y * y);
		}

		/**
		@brief	ベクトルの長さの2乗
		@return	求められた長さ
		*/
		inline float LengthSquared() const
		{
			return x * x + y * y + z * z;
		}

		/**
		@brief	XZの向き
		@return	向き
		*/
		inline float AngleXZ(const Vector3& v)
		{
			return atan2f(x - v.x, z - v.z);
		}

		/**
		@brief	XYの向き
		@return	向き
		*/
		inline float AngleXY(const Vector3& v)
		{
			return atan2f(x - v.x, y - v.y);
		}

		/**
		@brief	ベクトルの距離
		@return	2つのベクトルの距離
		*/
		float Distance(const Vector3& v) const 
		{
			return sqrtf((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) + (v.z - z) * (v.z - z));
		}

		/**
		@brief	XZ平面のベクトルの距離
		@return	2つのベクトルの距離(XZ平面)
		*/
		float DistanceXZ(const Vector3& v) const
		{
			return sqrtf((v.x - x) * (v.x - x) + (v.z - z) * (v.z - z));
		}

		/**
		@brief	ベクトルの距離の2乗
		@return	2つのベクトルの距離の2乗
		*/
		float DistSquared(const Vector3& v) const 
		{
			return (v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) + (v.z - z) * (v.z - z);
		}

		/**
		@brief	単位ベクトル
		@return	正規化されたベクトル
		*/
		inline Vector3 Normal()
		{
			Vector3 result;
			D3DXVec3Normalize(&result, this);
			return result;
		}

		/**
		@brief	外積
		@return	2つのベクトルの外積
		*/
		Vector3 Cross(const Vector3& v) const 
		{
			return Vector3(
				y * v.z - z * v.y,
				z * v.x - x * v.z,
				x * v.y - y * v.x);
		}

		/**
		@brief	内積
		@return	2つのベクトルの内積
		*/
		Vector3 Dot(const Vector3& v) const
		{
			return Vector3(x * v.x + y * v.y + z * v.z);
		}

		/**
		@brief	水平方向変換
		@return	2点の水平方向角度
		*/
		float GetHorizontalAngle(const Vector3& v) const 
		{
			return atan2f((x - v.x), (z - v.z));
		}

		/**
		@brief	ベクトルを反転
		@return	反転されたベクトル
		*/
		inline Vector3 Invert()
		{
			return Vector3(-x, -y, -z);
		}

		/**
		@brief	指定された許容誤差範囲内にあるか判定
		@param	range	[in]	許容範囲
		@return	判定結果
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
		@brief	要素がゼロか
		@return	判定結果
		*/
		inline bool IsZero() const
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
		@brief	文字列変換
		@return	(x, y, z)の形式で変換された文字列
		*/
		inline std::string ToString() const
		{
			std::stringstream text;
			text << "(" << x << ", " << y << ", " << z << ")";
			return text.str();
		}
	};

	/**
	@brief	HitResult【衝突情報】
	*/
	struct HitResult
	{
		bool ishit;				// 衝突結果
		MyLib::Vector3 hitpos;	// 衝突地点

		// デフォルトコンストラクタ
		HitResult() : ishit(false), hitpos() {}

		// パラメータ付きコンストラクタ
		HitResult(bool bHit, MyLib::Vector3 pos) : ishit(bHit), hitpos(pos) {}
	};

	/**
	@brief	HitResult_Character【キャラクターの衝突情報】
	*/
	struct HitResult_Character
	{
		bool ishit;				// 衝突結果
		bool isdamage;			// ダメージを喰らったか
		bool isdeath;			// 死亡したか
		MyLib::Vector3 hitpos;	// 衝突地点

		// デフォルトコンストラクタ
		HitResult_Character() : ishit(false), isdamage(false), isdeath(false), hitpos() {}

		// パラメータ付きコンストラクタ
		HitResult_Character(bool bHit, bool bDamage, bool bDeath, MyLib::Vector3 pos) : 
			ishit(bHit), isdamage(bDamage), isdeath(bDeath), hitpos(pos) {}
	};
}


// nlohmann::jsonの特殊化
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