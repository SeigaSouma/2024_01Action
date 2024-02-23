//=============================================================================
// 
// 便利関数 [calculation.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _CALCULATION_H_
#define _CALCULATION_H_		// 二重インクルード防止のマクロを定義する

#include "constans.h"

/**
@brief	簡単な説明
@details 詳細な説明
@param	hoge	[in][out][inout]	引数の説明
@return	piyo	戻り値の説明
*/

//==========================================================================
// マクロ定義
//==========================================================================
#define MAX_COMMENT	(512)	// コメントの最大数

/**
@brief	便利関数
*/
namespace UtilFunc
{
	namespace Transformation
	{
		template<class T> void ValueNormalize(T& Value, T MaxValue, T MinValue);	// 値の正規化処理
		MyLib::Vector3 RotationChangeToForwardVector(float rot);
	}

	namespace Collision
	{
		bool CollisionTriangle(const MyLib::Vector3& posCenter, const MyLib::Vector3& posLeft, const MyLib::Vector3& posRight, const MyLib::Vector3& MainPos, const MyLib::Vector3& MainPosOld);	// 三角形の内側にいるか判定

	}
}

//==========================================================================
// ネームスペース定義
//==========================================================================
namespace UtilFunc	// 便利関数
{

	/**
	@brief	補正関数
	*/
	namespace Correction
	{
		/**
		@brief	慣性補正
		@details 補正係数を1.0fにすると1Fで補正が完了します
		@param	fMain	[inout]	補正する値
		@param	fTarget	[in]	目標の値
		@param	fFactor	[in]	補正係数(0.0f ～ 1.0f)
		@return	void
		*/
		inline void InertiaCorrection(float& fMain, float fTarget, float fFactor)
		{
			// 範囲外補正
			UtilFunc::Transformation::ValueNormalize(fFactor, 1.0f, 0.0f);

			fMain += (fTarget - fMain) * fFactor;
		}

		/**
		@brief	慣性補正
		@details 補正係数を1.0fにすると1Fで補正が完了します
		@param	main	[inout]	補正する値
		@param	target	[in]	目標の値
		@param	fFactor	[in]	補正係数(0.0f ～ 1.0f)
		@return	void
		*/
		inline void InertiaCorrection(MyLib::Vector3& main, const MyLib::Vector3& target, float fFactor)
		{
			// 範囲外補正
			UtilFunc::Transformation::ValueNormalize(fFactor, 1.0f, 0.0f);

			main.x += (target.x - main.x) * fFactor;
			main.y += (target.y - main.y) * fFactor;
			main.z += (target.z - main.z) * fFactor;
		}

		/**
		@brief	線形補正(等速, リニア)
		@details https://mo-no.design/blog/wp-content/uploads/2023/02/linear.mp4
		@param	start	[in]	初期値
		@param	end		[in]	目標値
		@param	time	[in]	補正割合(0.0f ～ 1.0f)
		@return	補正されたfloat値
		*/
		inline float EasingLinear(float start, float end, float time)
		{
			// 範囲外補正
			UtilFunc::Transformation::ValueNormalize(time, 1.0f, 0.0f);
			return start + (end - start) * time;
		}

		/**
		@brief	線形補正(等速, リニア)
		@details https://mo-no.design/blog/wp-content/uploads/2023/02/linear.mp4
		@param	start		[in]	初期値
		@param	end			[in]	目標値
		@param	startTime	[in]	開始時間
		@param	endTime		[in]	終了時間
		@param	currentTime	[in]	現在の時間
		@return	補正されたfloat値
		*/
		inline float EasingLinear(float start, float end, float startTime, float endTime, float currentTime)
		{
			// 割合
			float ratio = (currentTime - startTime) / (endTime - startTime);
			UtilFunc::Transformation::ValueNormalize(ratio, 1.0f, 0.0f);

			return EasingLinear(start, end, ratio);
		}

		/**
		@brief	線形補正(徐々に加速, イーズイン)
		@details https://mo-no.design/blog/wp-content/uploads/2023/02/easein_1.mp4
		@param	start	[in]	初期値
		@param	end		[in]	目標値
		@param	time	[in]	補正割合(0.0f ～ 1.0f)
		@return	補正されたfloat値
		*/
		inline float EasingEaseIn(float start, float end, float time)
		{
			// 範囲外補正
			UtilFunc::Transformation::ValueNormalize(time, 1.0f, 0.0f);
			return start + (end - start) * time * time;
		}

		/**
		@brief	線形補正(徐々に加速, イーズイン)
		@details https://mo-no.design/blog/wp-content/uploads/2023/02/easein_1.mp4
		@param	start		[in]	初期値
		@param	end			[in]	目標値
		@param	startTime	[in]	開始時間
		@param	endTime		[in]	終了時間
		@param	currentTime	[in]	現在の時間
		@return	補正されたfloat値
		*/
		inline float EasingEaseIn(float start, float end, float startTime, float endTime, float currentTime)
		{
			// 割合
			float ratio = (currentTime - startTime) / (endTime - startTime);
			UtilFunc::Transformation::ValueNormalize(ratio, 1.0f, 0.0f);

			return EasingEaseIn(start, end, ratio);
		}

		/**
		@brief	線形補正(徐々に減速, イーズアウト)
		@details https://mo-no.design/blog/wp-content/uploads/2023/02/easeout_1.mp4
		@param	start	[in]	初期値
		@param	end		[in]	目標値
		@param	time	[in]	補正割合(0.0f ～ 1.0f)
		@return	補正されたfloat値
		*/
		inline float EasingEaseOut(float start, float end, float time)
		{
			// 範囲外補正
			UtilFunc::Transformation::ValueNormalize(time, 1.0f, 0.0f);
			float fSquaredTime = time * time;

			return start + (end - start) * (1.0f - (1.0f - fSquaredTime) * (1.0f - fSquaredTime));
		}

		/**
		@brief	線形補正(徐々に減速, イーズアウト)
		@details https://mo-no.design/blog/wp-content/uploads/2023/02/easeout_1.mp4
		@param	start		[in]	初期値
		@param	end			[in]	目標値
		@param	startTime	[in]	開始時間
		@param	endTime		[in]	終了時間
		@param	currentTime	[in]	現在の時間
		@return	補正されたfloat値
		*/
		inline float EasingEaseOut(float start, float end, float startTime, float endTime, float currentTime)
		{
			// 割合
			float ratio = (currentTime - startTime) / (endTime - startTime);
			UtilFunc::Transformation::ValueNormalize(ratio, 1.0f, 0.0f);

			return EasingEaseOut(start, end, ratio);
		}

		/**
		@brief	線形補正(加速してから減速, イーズインアウト)
		@details https://mo-no.design/blog/wp-content/uploads/2023/02/easeinout_1.mp4
		@param	start	[in]	初期値
		@param	end		[in]	目標値
		@param	time	[in]	補正割合(0.0f ～ 1.0f)
		@return	補正されたfloat値
		*/
		inline float EasingEaseInOut(float start, float end, float time)
		{
			// 範囲外補正
			UtilFunc::Transformation::ValueNormalize(time, 1.0f, 0.0f);

			float fValue = 0.0f;
			if (time < 0.5f)
			{
				time = 2.0f * time;
				fValue = start + (end - start) * 0.5f * time * time;
			}
			else 
			{
				time = 2.0f * time - 1.0f;
				float fSquaredTime = time * time;
				fValue = start + (end - start) * (0.5f * (1.0f - (1.0f - fSquaredTime) * (1.0f - fSquaredTime)) + 0.5f);
			}
			return fValue;
		}

		/**
		@brief	線形補正(加速してから減速, イーズインアウト)
		@details https://mo-no.design/blog/wp-content/uploads/2023/02/easeinout_1.mp4
		@param	start		[in]	初期値
		@param	end			[in]	目標値
		@param	startTime	[in]	開始時間
		@param	endTime		[in]	終了時間
		@param	currentTime	[in]	現在の時間
		@return	補正されたfloat値
		*/
		inline float EasingEaseInOut(float start, float end, float startTime, float endTime, float currentTime)
		{
			// 割合
			float ratio = (currentTime - startTime) / (endTime - startTime);
			UtilFunc::Transformation::ValueNormalize(ratio, 1.0f, 0.0f);

			return EasingEaseInOut(start, end, ratio);
		}

		/**
		@brief	Catmull-Rom Spline補間(全ての点を通る線形補間)
		@details https://storage.googleapis.com/zenn-user-upload/cu4a6qn5njn0pskn6pfx6lypic0h
		@param	PointMili1	[in]	基準の前の制御点(f-1)
		@param	Point0		[in]	基準の最初の制御点(f0)
		@param	Point1		[in]	基準の2番目の制御点(f1)
		@param	Point2		[in]	基準の次の制御点 (f2)
		@param	time		[in]	補正割合(0.0f ～ 1.0f)
		@return	補間された値
		*/
		inline MyLib::Vector3 CatmullRomSplineInterp(MyLib::Vector3 PointMili1, MyLib::Vector3 Point0, MyLib::Vector3 Point1, MyLib::Vector3 Point2, float time)
		{
			// 4点で補正
			MyLib::Vector3 a4 = Point0;
			MyLib::Vector3 a3 = (Point1 - PointMili1) / 2.0f;
			MyLib::Vector3 a1 = (Point2 - Point0) / 2.0f - 2.0f * Point1 + a3 + 2.0f * a4;
			MyLib::Vector3 a2 = 3.0f * Point1 - (Point2 - Point0) / 2.0f - 2.0f * a3 - 3.0f * a4;

			return (a1 * time * time * time + a2 * time * time + a3 * time + a4);
		}

		/**
		@brief	二次曲線補正
		@param	Value		[inout]	補完される値
		@param	nRange		[in]	補完の範囲
		@param	RangeOrigin	[in]	補完範囲の起点となる値
		@param	Max			[in]	補完の最大値
		@param	nAlphaCnt	[in]	二次曲線補正用のカウンター変数への参照
		@return	void
		*/
		template<class T> inline void CuadricCurveComp(T& Value, const int nRange, T RangeOrigin, T Max, int& nAlphaCnt)
		{
			// 補完の値割り出し
			T Comp = (static_cast<T>(Max) / static_cast<T>((nRange * nRange)));

			if (nAlphaCnt > nRange)
			{// 範囲超えたら戻す
				nAlphaCnt = -nRange;
			}
			else if (nAlphaCnt < -nRange)
			{// 範囲超えたら戻す
				nAlphaCnt = nRange;
			}

			// 不透明のカウント加算
			nAlphaCnt++;

			// 曲線補正
			Value = static_cast<T>(((Comp * (nAlphaCnt * nAlphaCnt)) - (2 * (Comp * nAlphaCnt * RangeOrigin)) + (Comp * (RangeOrigin * RangeOrigin))));
			Value += RangeOrigin;
		}

	}

	/**
	@brief	計算関数
	*/
	namespace Calculation
	{
		/**
		@brief	桁数取得
		@param	nNum	[in]	桁数を求めたい値
		@return	桁数
		*/
		inline int GetDigit(int nNum)
		{
			if (nNum <= 0)
			{
				nNum = 0;
			}

			// 整数を文字列に変換
			std::string number;
			sprintf((char*)number.c_str(), "%d", nNum);

			// 文字列の長さ（桁数）を取得
			int nDigit = strlen(number.c_str());
			if (nDigit <= 0)
			{
				nDigit = 1;
			}

			return nDigit;
		}

		/**
		@brief	指定された許容誤差範囲内にあるか判定
		@param	value	[in]	判定したい値
		@param	target	[in]	基準にする値
		@param	range	[in]	許容範囲
		@return	判定結果
		*/
		inline bool IsNearlyTarget(float value, float target, float range)
		{
			if (abs(value) <= range + target)
			{
				return true;
			}
			return false;
		}

		/**
		@brief	二点間の長さ取得(2D)
		@param	pos1	[in]	始点
		@param	pos2	[in]	終点
		@return	長さ
		*/
		inline float GetPosLength2D(MyLib::Vector3 pos1, MyLib::Vector3 pos2)
		{
			return sqrtf((pos1.x - pos2.x) * (pos1.x - pos2.x)
				+ (pos1.y - pos2.y) * (pos1.y - pos2.y));
		}

		/**
		@brief	二点間の長さの絶対値取得(2D)
		@param	pos1	[in]	始点
		@param	pos2	[in]	終点
		@return	長さの絶対値
		*/
		inline float GetFabsPosLength2D(MyLib::Vector3 pos1, MyLib::Vector3 pos2)
		{
			return fabsf(GetPosLength2D(pos1, pos2));
		}

		/**
		@brief	二点間の長さ取得(3D)
		@param	pos1	[in]	始点
		@param	pos2	[in]	終点
		@return	長さ
		*/
		inline float GetPosLength3D(MyLib::Vector3 pos1, MyLib::Vector3 pos2)
		{
			return sqrtf((pos1.x - pos2.x) * (pos1.x - pos2.x)
				+ (pos1.z - pos2.z) * (pos1.z - pos2.z));
		}

		/**
		@brief	二点間の長さの絶対値取得(3D)
		@param	pos1	[in]	始点
		@param	pos2	[in]	終点
		@return	長さの絶対値
		*/
		inline float GetFabsPosLength3D(MyLib::Vector3 pos1, MyLib::Vector3 pos2)
		{
			return fabsf(GetPosLength3D(pos1, pos2));
		}

		/**
		@brief	二点の中心点取得(3D)
		@param	pos1	[in]	始点
		@param	pos2	[in]	終点
		@return	中心点
		*/
		inline MyLib::Vector3 GetCenterPosition3D(MyLib::Vector3 pos1, MyLib::Vector3 pos2)
		{
			return MyLib::Vector3(
				(pos1.x + pos2.x),
				(pos1.y + pos2.y),
				(pos1.z + pos2.z)) * 0.5f;
		}

		/**
		@brief	辺上の位置取得(2D)
		@param	point1	[in]	始点の頂点座標
		@param	point2	[in]	終点の頂点座標
		@param	fRate	[in]	割合
		@return	位置
		*/
		inline MyLib::Vector3 GetPointOnEdge2D(MyLib::Vector3 point1, MyLib::Vector3 point2, float fRate)
		{
			return MyLib::Vector3(
				point1.x + fRate * (point2.x - point1.x),
				point1.y + fRate * (point2.y - point1.y),
				0.0f);
		}

		/**
		@brief	放物線の位置取得
		@param	start		[in]	始点
		@param	end			[in]	終点
		@param	fMaxHeight	[in]	高さの最大値
		@param	time		[in]	補正割合(0.0f ～ 1.0f)
		@return	放物線上の位置取得
		*/
		inline MyLib::Vector3 GetParabola3D(MyLib::Vector3 start, MyLib::Vector3 end, float fMaxHeight, float time)
		{
			// 位置
			MyLib::Vector3 pos = mylib_const::DEFAULT_VECTOR3;

			// XZ移動
			pos.x = start.x + (end.x - start.x) * time;
			pos.z = start.z + (end.z - start.z) * time;

			// 高さ
			pos.y = start.y + (end.y - start.y) * time + sinf(D3DX_PI * time) * fMaxHeight;

			return pos;
		}

		// 比率を計算する関数
		/**
		@brief	比率を計算
		@param	size		[in]	二次元サイズ
		@return	比率
		*/
		inline float AspectRatio(const D3DXVECTOR2& size) 
		{
			return size.x / size.y;
		}

		/**
		@brief	モデルの頂点情報計算
		@param	RotY		[in]	モデルのY軸向き
		@param	pVtxMax		[inout]	頂点座標の最大値
		@param	pVtxMin		[inout]	頂点座標の最小値
		@param	pMesh		[in]	モデルのメッシュオブジェクト
		@param	pVtxBuff	[in]	頂点バッファ
		@return	void
		*/
		inline void CalModelVtx(float RotY, MyLib::Vector3* pVtxMax, MyLib::Vector3* pVtxMin, LPD3DXMESH pMesh, BYTE* pVtxBuff)
		{
			// リセット
			*pVtxMin = MyLib::Vector3(10000.0f, 10000.0f, 10000.0f);
			*pVtxMax = MyLib::Vector3(-10000.0f, -10000.0f, -10000.0f);

			// モデルの頂点数を取得
			int nNumVtx = pMesh->GetNumVertices();

			// 頂点フォーマットのサイズを取得
			DWORD dwSizeFVF = D3DXGetFVFVertexSize(pMesh->GetFVF());

			// 頂点バッファをロック
			pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

			// 全ての頂点をチェックする
			for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
			{
				// 今回のデータ
				MyLib::Vector3 vtx = *(MyLib::Vector3*)pVtxBuff;

				// X
				if (pVtxMin->x > vtx.x)
				{// 今の最小値よりも今回の値がちいさかったら

					pVtxMin->x = vtx.x;
				}
				if (pVtxMax->x < vtx.x)
				{// 今の最大値よりも今回の値が大きかったら

					pVtxMax->x = vtx.x;
				}

				// Y
				if (pVtxMin->y > vtx.y)
				{// 今の最小値よりも今回の値がちいさかったら

					pVtxMin->y = vtx.y;
				}
				if (pVtxMax->y < vtx.y)
				{// 今の最大値よりも今回の値が大きかったら

					pVtxMax->y = vtx.y;
				}

				// Z
				if (pVtxMin->z > vtx.z)
				{// 今の最小値よりも今回の値がちいさかったら

					pVtxMin->z = vtx.z;
				}
				if (pVtxMax->z < vtx.z)
				{// 今の最大値よりも今回の値が大きかったら

					pVtxMax->z = vtx.z;
				}

				pVtxBuff += dwSizeFVF;	// サイズ分ポインタ移動
			}

			// 頂点バッファをアンロック
			pMesh->UnlockVertexBuffer();

			MyLib::Vector3 VtxMax = *pVtxMax, VtxMin = *pVtxMin;

			if (RotY <= D3DX_PI * 0.5f + 0.05f && RotY >= D3DX_PI * 0.5f - 0.05f)
			{// 1.57fのとき

				pVtxMax->x = VtxMax.z;
				pVtxMin->x = VtxMin.z;

				pVtxMax->z = -VtxMin.x;
				pVtxMin->z = -VtxMax.x;
			}
			else if (RotY <= -D3DX_PI * 0.5f + 0.05f && RotY >= -D3DX_PI * 0.5f - 0.05f)
			{// -1.57fのとき

				pVtxMax->x = -VtxMin.z;
				pVtxMin->x = -VtxMax.z;

				pVtxMax->z = VtxMax.x;
				pVtxMin->z = VtxMin.x;
			}
			else if (RotY <= D3DX_PI + 0.05f && RotY >= D3DX_PI - 0.05f)
			{// D3DX_PIのとき

				pVtxMax->x = -VtxMin.x;
				pVtxMin->x = -VtxMax.x;

				pVtxMax->z = -VtxMin.z;
				pVtxMin->z = -VtxMax.z;
			}
			else if (RotY <= -D3DX_PI + 0.05f && RotY >= -D3DX_PI - 0.05f)
			{// -D3DX_PIのとき

				pVtxMax->x = -VtxMin.x;
				pVtxMin->x = -VtxMax.x;

				pVtxMax->z = -VtxMin.z;
				pVtxMin->z = -VtxMax.z;
			}
		}

		/**
		@brief	3頂点からなる高さ取得
		@param	pos			[in]	判定するオブジェクトの位置
		@param	posCenter	[in]	判定する三角形の手前の頂点座標
		@param	posLeft		[in]	判定する三角形の左の頂点座標
		@param	posRight	[in]	判定する三角形の右の頂点座標
		@param	posRight	[out]	着地判定
		@return	3頂点からなる高さ
		*/
		inline float GetVtxHeight(const MyLib::Vector3& pos, const MyLib::Vector3& posCenter, const MyLib::Vector3& posRight, const MyLib::Vector3& posLeft, bool* pLand)
		{
			// 高さ
			float fHeight = 0.0f;

			// 直角部分と位置のベクトル
			MyLib::Vector3 calvec = pos - posCenter;

			// ベクトルと法線
			MyLib::Vector3
				vec1 = posRight - posCenter,
				vec2 = posLeft - posRight,
				nor = MyLib::Vector3(0.0f, 0.0f, 0.0f);

			if (UtilFunc::Collision::CollisionTriangle(posCenter, posRight, posLeft, pos, pos) == true)
			{// 三角に入っていたら

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = -((calvec.x * nor.x) + (-posCenter.y * nor.y) + (calvec.z * nor.z)) / nor.y;
					*pLand = true;
				}
				else
				{
					// 高さを求める
					fHeight = -((calvec.x * nor.x) + (-posCenter.y * nor.y) + (calvec.z * nor.z)) / nor.y;
					*pLand = true;
				}
			}

			return fHeight;
		}

		/**
		@brief	入れ替え処理
		@param	pValue		[out]	ソートする値
		@param	nStart		[in]	ソートする値の初期値
		@param	nMax		[in]	ソートの最大値
		@return	void
		*/
		template<class T> inline void CalSort(T* pValue, int nStart, int nMax)
		{
			for (int nCntData = nStart; nCntData < nMax - 1; nCntData++)
			{
				for (int nCntSort = nCntData + 1; nCntSort < nMax; nCntSort++)
				{
					T nTemp = pValue[nCntData];
					pValue[nCntData] = pValue[nCntSort];
					pValue[nCntSort] = nTemp;
				}
			}
		}
	}

	/**
	@brief	当たり判定関数
	*/
	namespace Collision
	{
		/**
		@brief	1と2の当たり判定(円)(2D)
		@param	pos1		[in]	対象1の位置
		@param	pos2		[in]	対象2の位置
		@param	fRadius1	[in]	対象1の半径
		@param	fRadius2	[in]	対象2の半径
		@return	当たったかのbool値
		*/
		inline bool CircleRange2D(const MyLib::Vector3& pos1, const MyLib::Vector3& pos2, float fRadius1, float fRadius2)
		{
			float fLength =
				(pos1.x - pos2.x) * (pos1.x - pos2.x) +
				(pos1.y - pos2.y) * (pos1.y - pos2.y);

			if (fLength <= (fRadius1 + fRadius2) * (fRadius1 + fRadius2))
			{// 円の中に入ったら
				return true;
			}

			return false;
		}

		/**
		@brief	1と2の当たり判定(円)(3D)
		@param	pos1		[in]	対象1の位置
		@param	pos2		[in]	対象2の位置
		@param	fRadius1	[in]	対象1の半径
		@param	fRadius2	[in]	対象2の半径
		@return	当たったかのbool値
		*/
		inline bool CircleRange3D(const MyLib::Vector3& pos1, const MyLib::Vector3& pos2, float fRadius1, float fRadius2)
		{
			float fLength =
				(pos1.x - pos2.x) * (pos1.x - pos2.x) +
				(pos1.z - pos2.z) * (pos1.z - pos2.z);

			if (fLength <= (fRadius1 + fRadius2) * (fRadius1 + fRadius2))
			{// 円の中に入ったら

				MyLib::Vector3 collisionPoint = pos1 + (pos2 - pos1).Normal() * fRadius1;

				return true;
			}

			return false;
		}

		/**
		@brief	1と2の当たり判定(球)
		@param	pos1		[in]	対象1の位置
		@param	pos2		[in]	対象2の位置
		@param	fRadius1	[in]	対象1の半径
		@param	fRadius2	[in]	対象2の半径
		@return	当たったかのbool値
		*/
		inline MyLib::HitResult SphereRange(const MyLib::Vector3& pos1, const MyLib::Vector3& pos2, float fRadius1, float fRadius2)
		{
			float fLength =
				(pos1.x - pos2.x) * (pos1.x - pos2.x) +
				(pos1.y - pos2.y) * (pos1.y - pos2.y) +
				(pos1.z - pos2.z) * (pos1.z - pos2.z);

			if (fLength <= (fRadius1 + fRadius2) * (fRadius1 + fRadius2))
			{// 球の中に入ったら

				MyLib::Vector3 collisionVector = (pos2 - pos1).Normal() * fRadius1;
				MyLib::Vector3 collisionPoint = pos1 + collisionVector;
				return MyLib::HitResult(true, collisionPoint);
			}

			return MyLib::HitResult(false, MyLib::Vector3());
		}

		/**
		@brief	1と2の当たり判定(筒)
		@param	pos1		[in]	対象1の位置
		@param	pos2		[in]	対象2の位置
		@param	fRadius1	[in]	対象1の半径
		@param	fRadius2	[in]	対象2の半径
		@param	fHeight1	[in]	対象1の高さ
		@param	fHeight2	[in]	対象2の高さ
		@return	当たったかのbool値
		*/
		inline bool CylinderRange(MyLib::Vector3 pos1, MyLib::Vector3 pos2, float fRadius1, float fRadius2, float fHeight1, float fHeight2)
		{
			if (CircleRange3D(pos1, pos2, fHeight1, fHeight2) &&
				pos1.y + fHeight1 >= pos2.y &&
				pos1.y <= pos2.y + fHeight2)
			{// 円の中に入る && 高さもぶつかる
				return true;
			}

			return false;
		}

		/**
		@brief	キャラクター同士の当たり判定 & 補正
		@param	MainPos			[inout]	自分の位置
		@param	MainPosOld		[inout]	自分の前回の位置
		@param	MainVtxMax		[in]	自分の頂点最大値
		@param	MainVtxMin		[in]	自分の頂点最小値
		@param	TargetPos		[inout]	対象の位置
		@param	TargetVtxMax	[in]	対象の頂点最大値
		@param	TargetVtxMin	[in]	対象の頂点最小値
		@return	void
		*/
		inline void CollisionCharacter(MyLib::Vector3& MainPos, MyLib::Vector3& MainPosOld, MyLib::Vector3 MainVtxMax, MyLib::Vector3 MainVtxMin, MyLib::Vector3& TargetPos, MyLib::Vector3 TargetVtxMax, MyLib::Vector3 TargetVtxMin)
		{
			// 横の判定
			if (MainPos.z + MainVtxMax.z > TargetPos.z + TargetVtxMin.z &&	// 敵の最小値に入ってる
				MainPos.z + MainVtxMin.z < TargetPos.z + TargetVtxMax.z)	// 敵の最大値に入ってる
			{// 敵のZ幅の中にいるとき

				if (MainPos.x + MainVtxMin.x <= TargetPos.x + TargetVtxMax.x &&		// 現在の最小値が敵の最大値よりも小さい
					MainPosOld.x + MainVtxMin.x >= TargetPos.x + TargetVtxMax.x)	// 前回の最小値が敵の最大値よりも大きい
				{// 右から

					MainPos.x = (TargetPos.x + TargetVtxMax.x) - MainVtxMin.x + 0.1f;
				}
				if (MainPos.x + MainVtxMax.x >= TargetPos.x + TargetVtxMin.x &&		// 現在の最大値が敵の最小値よりも大きい
					MainPosOld.x + MainVtxMax.x <= TargetPos.x + TargetVtxMin.x)	// 前回の最大値が敵の最小値よりも小さい
				{// 左から

					MainPos.x = (TargetPos.x + TargetVtxMin.x) - MainVtxMax.x - 0.1f;
				}
			}

			// 縦の判定
			if (MainPos.x + MainVtxMax.x > TargetPos.x + TargetVtxMin.x &&	// 敵の最小値に入ってる
				MainPos.x + MainVtxMin.x < TargetPos.x + TargetVtxMax.x)	// 敵の最大値に入ってる
			{// 敵のX幅の中にいるとき

				if (MainPos.z + MainVtxMin.z <= TargetPos.z + TargetVtxMax.z &&		// 現在の最小値が敵の最大値よりも小さい
					MainPosOld.z + MainVtxMin.z >= TargetPos.z + TargetVtxMax.z)	// 前回の最小値が敵の最大値よりも大きい
				{// 奥から

					MainPos.z = (TargetPos.z + TargetVtxMax.z) - MainVtxMin.z + 0.1f;
				}
				if (MainPos.z + MainVtxMax.z >= TargetPos.z + TargetVtxMin.z &&		// 現在の最大値が敵の最小値よりも大きい
					MainPosOld.z + MainVtxMax.z <= TargetPos.z + TargetVtxMin.z)	// 前回の最大値が敵の最小値よりも小さい
				{// 手前から

					MainPos.z = (TargetPos.z + TargetVtxMin.z) - MainVtxMax.z - 0.1f;
				}
			}
		}

		/**
		@brief	キャラクター同士の当たり判定
		@param	MainPos			[in]	自分の位置
		@param	MainPosOld		[in]	自分の前回の位置
		@param	MainVtxMax		[in]	自分の頂点最大値
		@param	MainVtxMin		[in]	自分の頂点最小値
		@param	TargetPos		[in]	対象の位置
		@param	TargetVtxMax	[in]	対象の頂点最大値
		@param	TargetVtxMin	[in]	対象の頂点最小値
		@return	当たったかのbool値
		*/
		inline bool bHitCharacter(MyLib::Vector3 MainPos, MyLib::Vector3 MainPosOld, MyLib::Vector3 MainVtxMax, MyLib::Vector3 MainVtxMin, MyLib::Vector3 TargetPos, MyLib::Vector3 TargetVtxMax, MyLib::Vector3 TargetVtxMin)
		{
			// 横の判定
			if (MainPos.z + MainVtxMax.z > TargetPos.z + TargetVtxMin.z &&	// 敵の最小値に入ってる
				MainPos.z + MainVtxMin.z < TargetPos.z + TargetVtxMax.z)	// 敵の最大値に入ってる
			{// 敵のZ幅の中にいるとき

				if (MainPos.x + MainVtxMin.x <= TargetPos.x + TargetVtxMax.x &&	// 現在の最小値が敵の最大値よりも小さい
					MainPosOld.x + MainVtxMin.x >= TargetPos.x + TargetVtxMax.x)// 前回の最小値が敵の最大値よりも大きい
				{// 右から
					return true;
				}
				if (MainPos.x + MainVtxMax.x >= TargetPos.x + TargetVtxMin.x &&	// 現在の最大値が敵の最小値よりも大きい
					MainPosOld.x + MainVtxMax.x <= TargetPos.x + TargetVtxMin.x)// 前回の最大値が敵の最小値よりも小さい
				{// 左から
					return true;
				}
			}

			// 縦の判定
			if (MainPos.x + MainVtxMax.x > TargetPos.x + TargetVtxMin.x &&	// 敵の最小値に入ってる
				MainPos.x + MainVtxMin.x < TargetPos.x + TargetVtxMax.x)	// 敵の最大値に入ってる
			{// 敵のX幅の中にいるとき

				if (MainPos.z + MainVtxMin.z <= TargetPos.z + TargetVtxMax.z &&	// 現在の最小値が敵の最大値よりも小さい
					MainPosOld.z + MainVtxMin.z >= TargetPos.z + TargetVtxMax.z)// 前回の最小値が敵の最大値よりも大きい
				{// 奥から
					return true;
				}
				if (MainPos.z + MainVtxMax.z >= TargetPos.z + TargetVtxMin.z &&	// 現在の最大値が敵の最小値よりも大きい
					MainPosOld.z + MainVtxMax.z <= TargetPos.z + TargetVtxMin.z)// 前回の最大値が敵の最小値よりも小さい
				{// 手前から
					return true;
				}
			}

			return false;
		}

		/**
		@brief	線分の当たり判定
		@param	pos0		[in]	線分左端の位置
		@param	pos1		[in]	線分右端の位置
		@param	pPos		[inout]	自分の位置
		@param	posOld		[in]	自分の前回位置
		@return	void
		*/
		inline void CollisionLimitLine(const MyLib::Vector3& pos0, const MyLib::Vector3& pos1, MyLib::Vector3& pPos, MyLib::Vector3 posOld)
		{
			// 境界線のベクトル
			MyLib::Vector3 vecLine;
			vecLine.x = pos1.x - pos0.x;
			vecLine.z = pos1.z - pos0.z;

			// プレイヤーの境界線のベクトル
			MyLib::Vector3 vecLinePlayer;
			vecLinePlayer.x = pPos.x - posOld.x;
			vecLinePlayer.z = pPos.z - posOld.z;

			// プレイヤーと壁のベクトル
			MyLib::Vector3 vecToPosPlayer;
			vecToPosPlayer.x = pos1.x - posOld.x;
			vecToPosPlayer.z = pos1.z - posOld.z;

			// 面積の最大値
			float fMaxAreaPlayer = (vecLinePlayer.z * vecLine.x) - (vecLinePlayer.x * vecLine.z);

			// 今回の面積
			float fNowAreaPlayer = (vecToPosPlayer.z * vecLine.x) - (vecToPosPlayer.x * vecLine.z);

			// 割合
			float fRatePlayer = fNowAreaPlayer / fMaxAreaPlayer;

			if (fRatePlayer >= 0.0f && fRatePlayer <= 1.0f)
			{// 面積の範囲内にいたら判定

				if ((vecLinePlayer.z * vecToPosPlayer.x) - (vecLinePlayer.x * vecToPosPlayer.z) > 0)
				{// 壁に当たったら

					// 法線ベクトル(境界線ベクトルのXとZ反転)
					MyLib::Vector3 vecNor = MyLib::Vector3(vecLine.z, 0.0f, -vecLine.x);

					// ベクトルの正規化
					D3DXVec3Normalize(&vecNor, &vecNor);

					// プレイヤーの逆移動量
					MyLib::Vector3 PlayerInverceMove;
					PlayerInverceMove.x = vecLinePlayer.x * (fRatePlayer - 1.0f);
					PlayerInverceMove.z = vecLinePlayer.z * (fRatePlayer - 1.0f);

					// 内積(壁の法線とプレイヤーの逆移動量)
					float fDot = (PlayerInverceMove.x * vecNor.x) + (PlayerInverceMove.z * vecNor.z);

					// 壁ずり移動量
					MyLib::Vector3 MoveWall = vecNor * fDot;

					// ぶつかった点に補正
					pPos += MoveWall + (vecNor * 0.1f);

				}
			}
		}

		/**
		@brief	線分の右にいるかどうか(2D)
		@details 線分に対して右にいるかの判定なので、左に居て欲しい場合はpos0とpos1を入れ替えて使用
		@param	pos0		[in]	線分左端の位置
		@param	pos1		[in]	線分右端の位置
		@param	MainPos		[in]	判定する対象の位置
		@param	MainPos		[in]	判定する対象の前回の位置
		@return	線分の右側にいるかのbool値
		*/
		inline bool CollisionLine2D(const MyLib::Vector3& pos0, const MyLib::Vector3& pos1, const MyLib::Vector3& MainPos, const MyLib::Vector3& MainPosOld)
		{
			// 境界線のベクトル
			MyLib::Vector3 vecLine;
			vecLine.x = pos1.x - pos0.x;
			vecLine.y = pos0.y - pos1.y;

			// 弾と壁のベクトル
			MyLib::Vector3 vecToPos;
			vecToPos.x = MainPos.x - pos0.x;
			vecToPos.y = pos0.y - MainPos.y;

			if ((vecLine.y * vecToPos.x) - (vecLine.x * vecToPos.y) <= 0)
			{// 線を超えた
				return true;
			}

			return false;
		}

		/**
		@brief	線分の右にいるかどうか(3D)
		@details 線分に対して右にいるかの判定なので、左に居て欲しい場合はpos0とpos1を入れ替えて使用
		@param	pos0		[in]	線分左端の位置
		@param	pos1		[in]	線分右端の位置
		@param	MainPos		[in]	判定する対象の位置
		@param	MainPos		[in]	判定する対象の前回の位置
		@return	線分の右側にいるかのbool値
		*/
		inline bool CollisionLine3D(const MyLib::Vector3& pos0, const MyLib::Vector3& pos1, const MyLib::Vector3& MainPos, const MyLib::Vector3& MainPosOld)
		{
			// 境界線のベクトル
			MyLib::Vector3 vecLine;
			vecLine.x = pos1.x - pos0.x;
			vecLine.z = pos1.z - pos0.z;

			// 弾と壁のベクトル
			MyLib::Vector3 vecToPos;
			vecToPos.x = MainPos.x - pos0.x;
			vecToPos.z = MainPos.z - pos0.z;

			if ((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z) <= 0)
			{// 線を超えた
				return true;
			}

			return false;
		}

		/**
		@brief	三角の内側にいるか判定
		@param	posCenter	[in]	判定する三角形の手前の頂点座標
		@param	posLeft		[in]	判定する三角形の左の頂点座標
		@param	posRight	[in]	判定する三角形の右の頂点座標
		@param	MainPos		[in]	自分の位置
		@param	MainPosOld	[in]	自分の前回位置
		@return	中にいるかのbool値
		*/
		inline bool CollisionTriangle(const MyLib::Vector3& posCenter, const MyLib::Vector3& posLeft, const MyLib::Vector3& posRight, const MyLib::Vector3& MainPos, const MyLib::Vector3& MainPosOld)
		{
			// 当たったかの判定
			bool bHit = false;
			bool bLine1 = false, bLine2 = false, bLine3 = false;

			// 真ん中と左の線
			bLine1 = UtilFunc::Collision::CollisionLine3D(posCenter, posLeft, MainPos, MainPosOld);

			// 左と右の線
			bLine2 = UtilFunc::Collision::CollisionLine3D(posLeft, posRight, MainPos, MainPosOld);

			// 右と真ん中の線
			bLine3 = UtilFunc::Collision::CollisionLine3D(posRight, posCenter, MainPos, MainPosOld);

			// 全ての範囲に入っていたら
			if (bLine1 == true && bLine2 == true && bLine3 == true)
			{
				bHit = true;
			}

			return bHit;
		}

		/**
		@brief	四角の内側にいるか判定
		@details 4つの頂点が全て決まっている場合に使う
		@param	posLeftUP	[in]	判定する四角形左上の頂点座標
		@param	posRightUP	[in]	判定する四角形右上の頂点座標
		@param	posLeftDW	[in]	判定する四角形左下の頂点座標
		@param	posRightDW	[in]	判定する四角形右下の頂点座標
		@param	MainPos		[in]	自分の位置
		@return	中にいるかのbool値
		*/
		inline bool CollisionSquare(const MyLib::Vector3& posLeftUP, const MyLib::Vector3& posRightUP, const MyLib::Vector3& posLeftDW, const MyLib::Vector3& posRightDW, const MyLib::Vector3& MainPos)
		{
			// 当たったかの判定
			bool bHit = false;
			bool bLine1 = false, bLine2 = false, bLine3 = false, bLine4 = false;

			// 左上と右上
			bLine1 = UtilFunc::Collision::CollisionLine3D(posRightUP, posLeftUP, MainPos, MainPos);

			// 右上と右下
			bLine2 = UtilFunc::Collision::CollisionLine3D(posRightDW, posRightUP, MainPos, MainPos);

			// 右下と左下
			bLine3 = UtilFunc::Collision::CollisionLine3D(posLeftDW, posRightDW, MainPos, MainPos);

			// 左下と左上
			bLine4 = UtilFunc::Collision::CollisionLine3D(posLeftUP, posLeftDW, MainPos, MainPos);

			// 全ての範囲に入っていたら
			if (bLine1 == true && bLine2 == true && bLine3 == true && bLine4 == true)
			{
				// 当たった
				bHit = true;
			}

			return bHit;
		}

		/**
		@brief	四角の内側にいるか判定
		@details 4つの頂点がまだ求められていない場合に使う
		@param	posSquare	[in]	判定する四角形の中心座標
		@param	SquareSize	[in]	判定する四角形のサイズ
		@param	fSquareRot	[in]	判定する四角形のサイズ
		@param	MainPos		[in]	自分の位置
		@return	中にいるかのbool値
		*/
		inline bool CollisionSquare(MyLib::Vector3 posSquare, D3DXVECTOR2 SquareSize, float fSquareRot, MyLib::Vector3 MainPos)
		{
			// 当たったかの判定
			bool bHit = false;
			bool bLine1 = false, bLine2 = false, bLine3 = false, bLine4 = false;

			float fLength = sqrtf(SquareSize.x * SquareSize.x + SquareSize.y * SquareSize.y);	// 対角線の長さ
			float fAngle = atan2f(SquareSize.x, SquareSize.y);									// 対角線の向き

			// 判定する四角の4頂点
			MyLib::Vector3 LeftUp = MyLib::Vector3(posSquare.x + sinf(fSquareRot - fAngle) * fLength, posSquare.y, posSquare.z + cosf(fSquareRot - fAngle) * fLength);
			MyLib::Vector3 RightUp = MyLib::Vector3(posSquare.x + sinf(fSquareRot + fAngle) * fLength, posSquare.y, posSquare.z + cosf(fSquareRot + fAngle) * fLength);
			MyLib::Vector3 LeftDown = MyLib::Vector3(posSquare.x + sinf(fSquareRot - D3DX_PI + fAngle) * fLength, posSquare.y, posSquare.z + cosf(fSquareRot - D3DX_PI + fAngle) * fLength);
			MyLib::Vector3 RightDown = MyLib::Vector3(posSquare.x + sinf(fSquareRot + D3DX_PI - fAngle) * fLength, posSquare.y, posSquare.z + cosf(fSquareRot + D3DX_PI - fAngle) * fLength);

			// 左上と右上
			bLine1 = UtilFunc::Collision::CollisionLine3D(RightUp, LeftUp, MainPos, MainPos);

			// 右上と右下
			bLine2 = UtilFunc::Collision::CollisionLine3D(RightDown, RightUp, MainPos, MainPos);

			// 右下と左下
			bLine3 = UtilFunc::Collision::CollisionLine3D(LeftDown, RightDown, MainPos, MainPos);

			// 左下と左上
			bLine4 = UtilFunc::Collision::CollisionLine3D(LeftUp, LeftDown, MainPos, MainPos);

			// 全ての範囲に入っていたら
			if (bLine1 == true && bLine2 == true && bLine3 == true && bLine4 == true)
			{
				// 当たった
				bHit = true;
			}

			return bHit;
		}

		/**
		@brief	四角の内側にいるか判定(2D)
		@details 4つの頂点が全て決まっている場合に使う
		@param	posLeftUP	[in]	判定する四角形左上の頂点座標
		@param	posRightUP	[in]	判定する四角形右上の頂点座標
		@param	posLeftDW	[in]	判定する四角形左下の頂点座標
		@param	posRightDW	[in]	判定する四角形右下の頂点座標
		@param	MainPos		[in]	自分の位置
		@return	中にいるかのbool値
		*/
		inline bool CollisionSquare2D(MyLib::Vector3 posLeftUP, MyLib::Vector3 posRightUP, MyLib::Vector3 posLeftDW, MyLib::Vector3 posRightDW, MyLib::Vector3 MainPos)
		{
			// 当たったかの判定
			bool bHit = false;
			bool bLine1 = false, bLine2 = false, bLine3 = false, bLine4 = false;

			// 左上と右上
			bLine1 = UtilFunc::Collision::CollisionLine2D(posRightUP, posLeftUP, MainPos, MainPos);

			// 右上と右下
			bLine2 = UtilFunc::Collision::CollisionLine2D(posRightDW, posRightUP, MainPos, MainPos);

			// 右下と左下
			bLine3 = UtilFunc::Collision::CollisionLine2D(posLeftDW, posRightDW, MainPos, MainPos);

			// 左下と左上
			bLine4 = UtilFunc::Collision::CollisionLine2D(posLeftUP, posLeftDW, MainPos, MainPos);

			// 全ての範囲に入っていたら
			if (bLine1 == true && bLine2 == true && bLine3 == true && bLine4 == true)
			{
				bHit = true;
			}

			return bHit;
		}

		/**
		@brief	四角と四角の当たり判定(2D)
		@details 4つの頂点がまだ求められていない場合に使う
		@param	MainPos		[inout]	自分の位置
		@param	TargetPos	[inout]	対象の位置
		@param	MainSize	[in]	自分のサイズ
		@param	TargetSize	[in]	対象のサイズ
		@param	fMainRot	[in]	自分のY軸の向き
		@param	fTargetRot	[in]	対象のY軸の向き
		@return	衝突したかのbool値
		*/
		inline bool CollisionSquareSquare2D(MyLib::Vector3& MainPos, MyLib::Vector3& TargetPos, D3DXVECTOR2 MainSize, D3DXVECTOR2 TargetSize, float fMainRot, float fTargetRot)
		{
			float fTargetLength = sqrtf(
				TargetSize.x * TargetSize.x +
				TargetSize.y * TargetSize.y);							// 対角線の長さ
			float fTargetAngle = atan2f(TargetSize.x, TargetSize.y);	// 対角線の向き

			// 判定する四角の4頂点
			MyLib::Vector3 LeftUp = MyLib::Vector3(
				TargetPos.x + sinf(fTargetRot - fTargetAngle) * fTargetLength,
				TargetPos.y,
				TargetPos.z + cosf(fTargetRot - fTargetAngle) * fTargetLength);

			MyLib::Vector3 RightUp = MyLib::Vector3(
				TargetPos.x + sinf(fTargetRot + fTargetAngle) * fTargetLength,
				TargetPos.y,
				TargetPos.z + cosf(fTargetRot + fTargetAngle) * fTargetLength);

			MyLib::Vector3 LeftDown = MyLib::Vector3(
				TargetPos.x + sinf(fTargetRot - D3DX_PI + fTargetAngle) * fTargetLength,
				TargetPos.y,
				TargetPos.z + cosf(fTargetRot - D3DX_PI + fTargetAngle) * fTargetLength);

			MyLib::Vector3 RightDown = MyLib::Vector3(
				TargetPos.x + sinf(fTargetRot + D3DX_PI - fTargetAngle) * fTargetLength,
				TargetPos.y,
				TargetPos.z + cosf(fTargetRot + D3DX_PI - fTargetAngle) * fTargetLength);

			//***********************
			// 矩形の判定
			//***********************
			if (UtilFunc::Collision::CollisionSquare(MainPos, MainSize, fMainRot, LeftUp) == true)
			{// 左上
				return true;
			}
			if (UtilFunc::Collision::CollisionSquare(MainPos, MainSize, fMainRot, RightUp) == true)
			{// 右上
				return true;
			}
			if (UtilFunc::Collision::CollisionSquare(MainPos, MainSize, fMainRot, LeftDown) == true)
			{// 左下
				return true;
			}
			if (UtilFunc::Collision::CollisionSquare(MainPos, MainSize, fMainRot, RightDown) == true)
			{// 右下
				return true;
			}

			return false;
		}

		/**
		@brief	円と矩形の当たり判定(2D)
		@param	posCircle		[inout]	円の中心座標
		@param	posSquare		[inout]	矩形の中心座標
		@param	rotSquare		[in]	矩形の向き
		@param	fCircleRadius	[in]	円の半径
		@param	SquareSize		[in]	矩形のサイズ
		@return	衝突したかのbool値
		*/
		inline bool CollisionCircleSquare2D(MyLib::Vector3& posCircle, MyLib::Vector3& posSquare, MyLib::Vector3 rotSquare, float fCircleRadius, D3DXVECTOR2 SquareSize)
		{
			float fSquareSizeX = SquareSize.x;	// 矩形のサイズX
			float fSquareSizeY = SquareSize.y;	// 矩形のサイズY

			float fLength = 0.0f;	// 対角線の長さ
			float fAngle = 0.0f;	// 対角線の向き

			MyLib::Vector3 LeftUp, RightUp, LeftDown, RightDown;

			//****************************************************
			// Xのサイズに円の半径を足した矩形の判定
			//****************************************************
			fSquareSizeX += fCircleRadius;	// 矩形のサイズX
			fSquareSizeY = SquareSize.y;	// 矩形のサイズY

			fLength = sqrtf(fSquareSizeX * fSquareSizeX + fSquareSizeY * fSquareSizeY);	// 対角線の長さ
			fAngle = atan2f(fSquareSizeX, fSquareSizeY);								// 対角線の向き

			// 判定する四角の4頂点
			LeftUp.x = posSquare.x + sinf(rotSquare.z - D3DX_PI + fAngle) * fLength;
			LeftUp.y = posSquare.y + cosf(rotSquare.z - D3DX_PI + fAngle) * fLength;
			LeftUp.z = 0.0f;

			RightUp.x = posSquare.x + sinf(rotSquare.z + D3DX_PI - fAngle) * fLength;
			RightUp.y = posSquare.y + cosf(rotSquare.z + D3DX_PI - fAngle) * fLength;
			RightUp.z = 0.0f;

			LeftDown.x = posSquare.x + sinf(rotSquare.z - fAngle) * fLength;
			LeftDown.y = posSquare.y + cosf(rotSquare.z - fAngle) * fLength;
			LeftDown.z = 0.0f;

			RightDown.x = posSquare.x + sinf(rotSquare.z + fAngle) * fLength;
			RightDown.y = posSquare.y + cosf(rotSquare.z + fAngle) * fLength;
			RightDown.z = 0.0f;

			// 矩形の判定
			if (UtilFunc::Collision::CollisionSquare2D(LeftUp, RightUp, LeftDown, RightDown, posCircle) == true)
			{// 矩形に当たった場合
				return true;
			}

			//****************************************************
			// Yのサイズに円の半径を足した矩形の判定
			//****************************************************
			fSquareSizeX = SquareSize.x;	// 矩形のサイズX
			fSquareSizeY += fCircleRadius;	// 矩形のサイズY

			fLength = sqrtf(fSquareSizeX * fSquareSizeX + fSquareSizeY * fSquareSizeY);	// 対角線の長さ
			fAngle = atan2f(fSquareSizeX, fSquareSizeY);								// 対角線の向き

			// 判定する四角の4頂点
			LeftUp.x = posSquare.x + sinf(rotSquare.z - D3DX_PI + fAngle) * fLength;
			LeftUp.y = posSquare.y + cosf(rotSquare.z - D3DX_PI + fAngle) * fLength;
			LeftUp.z = 0.0f;

			RightUp.x = posSquare.x + sinf(rotSquare.z + D3DX_PI - fAngle) * fLength;
			RightUp.y = posSquare.y + cosf(rotSquare.z + D3DX_PI - fAngle) * fLength;
			RightUp.z = 0.0f;

			LeftDown.x = posSquare.x + sinf(rotSquare.z - fAngle) * fLength;
			LeftDown.y = posSquare.y + cosf(rotSquare.z - fAngle) * fLength;
			LeftDown.z = 0.0f;

			RightDown.x = posSquare.x + sinf(rotSquare.z + fAngle) * fLength;
			RightDown.y = posSquare.y + cosf(rotSquare.z + fAngle) * fLength;
			RightDown.z = 0.0f;

			// 矩形の判定
			if (UtilFunc::Collision::CollisionSquare2D(LeftUp, RightUp, LeftDown, RightDown, posCircle) == true)
			{// 矩形に当たった場合
				return true;
			}

			//***********************
			// 各頂点毎の円の判定
			//***********************
			//****************************************************
			// 元のサイズ
			//****************************************************
			fSquareSizeX = SquareSize.x;	// 矩形のサイズX
			fSquareSizeY = SquareSize.y;	// 矩形のサイズY

			fLength = sqrtf(fSquareSizeX * fSquareSizeX + fSquareSizeY * fSquareSizeY);	// 対角線の長さ
			fAngle = atan2f(fSquareSizeX, fSquareSizeY);								// 対角線の向き

			// 判定する四角の4頂点
			LeftUp.x = posSquare.x + sinf(rotSquare.z - D3DX_PI + fAngle) * fLength;
			LeftUp.y = posSquare.y + cosf(rotSquare.z - D3DX_PI + fAngle) * fLength;
			LeftUp.z = 0.0f;

			RightUp.x = posSquare.x + sinf(rotSquare.z + D3DX_PI - fAngle) * fLength;
			RightUp.y = posSquare.y + cosf(rotSquare.z + D3DX_PI - fAngle) * fLength;
			RightUp.z = 0.0f;

			LeftDown.x = posSquare.x + sinf(rotSquare.z - fAngle) * fLength;
			LeftDown.y = posSquare.y + cosf(rotSquare.z - fAngle) * fLength;
			LeftDown.z = 0.0f;

			RightDown.x = posSquare.x + sinf(rotSquare.z + fAngle) * fLength;
			RightDown.y = posSquare.y + cosf(rotSquare.z + fAngle) * fLength;
			RightDown.z = 0.0f;

			if (UtilFunc::Collision::CircleRange2D(posCircle, LeftUp, fCircleRadius, 0.0f) == true)
			{// 左上
				return true;
			}
			if (UtilFunc::Collision::CircleRange2D(posCircle, RightUp, fCircleRadius, 0.0f) == true)
			{// 右上
				return true;
			}
			if (UtilFunc::Collision::CircleRange2D(posCircle, LeftDown, fCircleRadius, 0.0f) == true)
			{// 左下
				return true;
			}
			if (UtilFunc::Collision::CircleRange2D(posCircle, RightDown, fCircleRadius, 0.0f) == true)
			{// 右下
				return true;
			}

			// 当たってない判定を返す
			return false;
		}

		/**
		@brief	円と矩形の当たり判定(3D)
		@param	posCircle		[inout]	円の中心座標
		@param	posSquare		[inout]	矩形の中心座標
		@param	rotSquare		[in]	矩形の向き
		@param	fCircleRadius	[in]	円の半径
		@param	SquareSize		[in]	矩形のサイズ
		@return	衝突したかのbool値
		*/
		inline bool CollisionCircleSquare3D(MyLib::Vector3& posCircle, MyLib::Vector3& posSquare, MyLib::Vector3 rotSquare, float fCircleRadius, D3DXVECTOR2 SquareSize)
		{
			float fSquareSizeX = SquareSize.x;	// 矩形のサイズX
			float fSquareSizeY = SquareSize.y;	// 矩形のサイズY

			float fLength = 0.0f;	// 対角線の長さ
			float fAngle = 0.0f;	// 対角線の向き

			MyLib::Vector3 LeftUp, RightUp, LeftDown, RightDown;

			//****************************************************
			// Xのサイズに円の半径を足した矩形の判定
			//****************************************************
			fSquareSizeX += fCircleRadius;	// 矩形のサイズX
			fSquareSizeY = SquareSize.y;	// 矩形のサイズY

			fLength = sqrtf(fSquareSizeX * fSquareSizeX + fSquareSizeY * fSquareSizeY);	// 対角線の長さ
			fAngle = atan2f(fSquareSizeX, fSquareSizeY);								// 対角線の向き

			// 判定する四角の4頂点
			LeftUp = MyLib::Vector3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y - fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y - fAngle) * fLength);
			RightUp = MyLib::Vector3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y + fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y + fAngle) * fLength);
			LeftDown = MyLib::Vector3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y - D3DX_PI + fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y - D3DX_PI + fAngle) * fLength);
			RightDown = MyLib::Vector3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y + D3DX_PI - fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y + D3DX_PI - fAngle) * fLength);

			// 矩形の判定
			if (UtilFunc::Collision::CollisionSquare(LeftUp, RightUp, LeftDown, RightDown, posCircle) == true)
			{// 矩形に当たった場合
				return true;
			}

			//****************************************************
			// Yのサイズに円の半径を足した矩形の判定
			//****************************************************
			fSquareSizeX = SquareSize.x;	// 矩形のサイズX
			fSquareSizeY += fCircleRadius;	// 矩形のサイズY

			fLength = sqrtf(fSquareSizeX * fSquareSizeX + fSquareSizeY * fSquareSizeY);	// 対角線の長さ
			fAngle = atan2f(fSquareSizeX, fSquareSizeY);								// 対角線の向き

			// 判定する四角の4頂点
			LeftUp = MyLib::Vector3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y - fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y - fAngle) * fLength);
			RightUp = MyLib::Vector3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y + fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y + fAngle) * fLength);
			LeftDown = MyLib::Vector3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y - D3DX_PI + fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y - D3DX_PI + fAngle) * fLength);
			RightDown = MyLib::Vector3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y + D3DX_PI - fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y + D3DX_PI - fAngle) * fLength);

			// 矩形の判定
			if (UtilFunc::Collision::CollisionSquare(LeftUp, RightUp, LeftDown, RightDown, posCircle) == true)
			{// 矩形に当たった場合
				return true;
			}

			//***********************
			// 各頂点毎の円の判定
			//***********************
			if (UtilFunc::Collision::CircleRange3D(posCircle, LeftUp, fCircleRadius, fCircleRadius) == true)
			{// 左上
				return true;
			}
			if (UtilFunc::Collision::CircleRange3D(posCircle, RightUp, fCircleRadius, fCircleRadius) == true)
			{// 右上
				return true;
			}
			if (UtilFunc::Collision::CircleRange3D(posCircle, LeftDown, fCircleRadius, fCircleRadius) == true)
			{// 左下
				return true;
			}
			if (UtilFunc::Collision::CircleRange3D(posCircle, RightDown, fCircleRadius, fCircleRadius) == true)
			{// 右下
				return true;
			}

			// 当たってない判定を返す
			return false;
		}

		/**
		@brief	扇の当たり判定(3D)
		@param	posCenter		[in]	扇の始点頂点座標
		@param	posLeft			[in]	扇の左側頂点座標
		@param	posRight		[in]	扇の右側頂点座標
		@param	TargetPos		[in]	判定する対象の位置
		@param	fTargetRadius	[in]	判定する対象の半径
		@return	衝突したかのbool値
		*/
		inline bool CollisionFan3D(MyLib::Vector3 posCenter, MyLib::Vector3 posLeft, MyLib::Vector3 posRight, MyLib::Vector3 TargetPos, float fTargetRadius)
		{
			// 当たったかの判定
			bool bHit = false;
			bool bLine1 = false, bLine2 = false, bCircle = false;

			// 左の点との線分
			bLine1 = UtilFunc::Collision::CollisionLine3D(posLeft, posCenter, TargetPos, TargetPos);

			// 右の点との線分
			bLine2 = UtilFunc::Collision::CollisionLine3D(posCenter, posRight, TargetPos, TargetPos);

			// 扇の半径
			float fLen =
				sqrtf((posLeft.x - posCenter.x) * (posLeft.x - posCenter.x)
					+ (posLeft.z - posCenter.z) * (posLeft.z - posCenter.z));

			// 円の当たり判定
			if (UtilFunc::Collision::CircleRange3D(posCenter, TargetPos, fLen, fTargetRadius) == true)
			{// 当たっていたら
				bCircle = true;
			}

			// 全ての範囲に入っていたら
			if (bLine1 == true && bLine2 == true && bCircle == true)
			{
				bHit = true;
			}

			return bHit;
		}

		/**
		@brief	視界内の判定(3D)
		@param	posMain			[in]	中心となる人の位置
		@param	posTarget		[in]	対象の位置
		@param	mainRotY		[in]	向き
		@param	fieldofview		[in]	視野角
		@return	衝突したかのbool値
		*/
		inline bool CollisionViewRange3D(const MyLib::Vector3& posMain, MyLib::Vector3& posTarget, float mainRotY, float fieldofview)
		{

			// 2キャラのベクトル
			D3DXVECTOR3 toPlayer = posTarget - posMain;

			// 前方ベクトル
			D3DXVECTOR3 forward = UtilFunc::Transformation::RotationChangeToForwardVector(mainRotY);

			// ベクトルと前方ベクトルの角度計算
			float angle = D3DXVec3Dot(&toPlayer, &forward) / (D3DXVec3Length(&toPlayer) * D3DXVec3Length(&forward));
			angle = acos(angle);

			// 視界の中にいるか判定
			fieldofview = D3DXToRadian(fieldofview);
			if (angle <= fieldofview / 2.0f)
			{
				return true;
			}

			return false;
		}
	}

	/**
	@brief	変換関数
	*/
	namespace Transformation
	{
		/**
		@brief	角度の正規化(単軸)
		@details -π or πを超えた角度を補正する。範囲内に収まるまで無限ループ
		@param	fRot	[inout]	補正する角度
		@return	void
		*/
		inline void RotNormalize(float& fRot)
		{
			while (1)
			{
				// 1周分越えたら補正する
				if (fRot < -D3DX_PI)
				{
					fRot += D3DX_PI * 2.0f;
				}
				else if (fRot > D3DX_PI)
				{
					fRot -= D3DX_PI * 2.0f;
				}

				if (fRot >= -D3DX_PI && fRot <= D3DX_PI)
				{// 全て正規化済みで終了
					break;
				}
			}
		}

		/**
		@brief	角度の正規化(VECTOR3)
		@details -π or πを超えた角度を補正する。範囲内に収まるまで無限ループ
		@param	fRot	[inout]	補正する角度
		@return	void
		*/
		inline void RotNormalize(MyLib::Vector3& rot)
		{
			// それぞれの正規化
			UtilFunc::Transformation::RotNormalize(rot.x);
			UtilFunc::Transformation::RotNormalize(rot.y);
			UtilFunc::Transformation::RotNormalize(rot.z);
		}

		/**
		@brief	度数法をラジアンに変換
		@param	degree	[in]	度数法の角度
		@return	ラジアン角度
		*/
		inline float DegreeChangeToRadian(float degree)
		{
			return degree * (D3DX_PI / 180.0f);
		}

		/**
		@brief	ラジアン角度を度数法に変換
		@param	radian	[in]	ラジアン角度
		@return	度数法
		*/
		inline float RadianChangeToDegree(float radian)
		{
			return radian * (180.0f / D3DX_PI);
		}

		/**
		@brief	値の正規化処理
		@param	Value		[inout]	補正したい値
		@param	MaxValue	[in]	最大値
		@param	MinValue	[in]	最小値
		@return	void
		*/
		template<class T>inline void ValueNormalize(T& Value, T MaxValue, T MinValue)
		{
			if (Value >= MaxValue)
			{
				// 最大値に補正
				Value = MaxValue;
			}
			else if (Value <= MinValue)
			{
				// 最小値に補正
				Value = MinValue;
			}
		}

		/**
		@brief	ワールドマトリックスをposに変換
		@param	worldmtx	[in]	ワールドマトリックス
		@return	位置
		*/
		inline MyLib::Vector3 WorldMtxChangeToPosition(D3DXMATRIX worldmtx)
		{
			return MyLib::Vector3(worldmtx._41, worldmtx._42, worldmtx._43);
		}

		/**
		@brief	ワールドマトリックスをrotに変換
		@param	worldmtx	[in]	ワールドマトリックス
		@return	向き(絶対値)
		*/
		inline MyLib::Vector3 WorldMtxChangeToRotation(D3DXMATRIX worldmtx)
		{
			MyLib::Vector3 rot = mylib_const::DEFAULT_VECTOR3;

			rot.x = atan2f(worldmtx._32, worldmtx._33);
			rot.y = atan2f(-worldmtx._31, sqrtf(worldmtx._32 * worldmtx._32 + worldmtx._33 * worldmtx._33));
			rot.z = atan2f(worldmtx._21, worldmtx._11);

			// XYZ 順序 (ロール → ピッチ → ヨー)
			/*rot.x = atan2f(-worldmtx._23, worldmtx._22);
			rot.y = asinf(worldmtx._21);
			rot.z = atan2f(-worldmtx._12, worldmtx._11);*/

			// ZYX 順序 (ヨー → ピッチ → ロール)
			// rot.z = atan2f(worldmtx._21, worldmtx._11);
			// rot.y = asinf(-worldmtx._31);
			// rot.x = atan2f(worldmtx._32, worldmtx._33);

			// ZYZ 順序 (ヨー → ピッチ → ヨー)
			// rot.z = atan2f(worldmtx._32, worldmtx._31);
			// rot.y = acosf(worldmtx._33);
			// rot.z = atan2f(worldmtx._23, -worldmtx._13);
			return rot;
		}

		/**
		@brief	向きを前方ベクトルに変換
		@param	rot	[in]	向き
		@return	前方ベクトル
		*/
		inline MyLib::Vector3 RotationChangeToForwardVector(float rot)
		{
			return MyLib::Vector3(sinf(D3DX_PI + rot), 0.0f, cosf(D3DX_PI + rot));
		}

		/**
		@brief	ランダム値取得
		@param	nMinNum	[in]	範囲の最小値
		@param	nMaxNum	[in]	範囲の最大値
		@return	抽選されたランダム値
		*/
		inline int Random(int nMinNum, int nMaxNum)
		{
			// 範囲
			int nRange = nMaxNum - nMinNum;

			if (nMinNum == 0)
			{
				// 範囲に1加算
				nRange += 1;
			}

			if (nMinNum + 1 == nMaxNum || nMaxNum - 1 == nMinNum)
			{
				nRange += 1;
			}

			// ランダム生成
			return (rand() % nRange + nMinNum);
		}

		/**
		@brief	ランダムな円周率取得
		@return	抽選されたランダム値
		*/
		inline float GetRandomPi()
		{
			return static_cast<float>(UtilFunc::Transformation::Random(-314, 314) * 0.01f);
		}

		/**
		@brief	球範囲のランダムベクトル取得
		@return	球のランダムなベクトル値
		*/
		inline MyLib::Vector3 GetRandomVecSphere()
		{
			// 球ベクトル
			MyLib::Vector3 vecSphere = mylib_const::DEFAULT_VECTOR3;

			// 球の範囲内でランダム生成
			float fRot = UtilFunc::Transformation::GetRandomPi(), fRotPhi = UtilFunc::Transformation::GetRandomPi();

			vecSphere.x = cosf(fRot) * sinf(fRotPhi);
			vecSphere.y = sinf(fRot);
			vecSphere.z = cosf(fRot) * cosf(fRotPhi);

			return vecSphere;
		}

		/**
		@brief	球範囲のランダムな位置取得
		@param	pos		[in]	球の中心座標
		@param	fRadius	[in]	取得する球の半径
		@return	抽選されたランダムな位置
		*/
		inline MyLib::Vector3 GetRandomPositionSphere(MyLib::Vector3 pos, float fRadius)
		{
			MyLib::Vector3 spherepos = mylib_const::DEFAULT_VECTOR3;
			MyLib::Vector3 vec = GetRandomVecSphere();

			// 引数情報計算
			spherepos = vec * fRadius;
			spherepos += pos;

			return spherepos;
		}

		/**
		@brief	高さを基準に幅を自動変換
		@param	size		[in]	元のサイズ
		@param	height		[in]	変更後の高さ
		@return	二次元サイズ
		*/
		inline D3DXVECTOR2 AdjustSizeByHeight(D3DXVECTOR2 size, float height)
		{
			D3DXVECTOR2 result = size;
			result.y = height;

			// 高さと比率から幅を計算
			result.x = height * UtilFunc::Calculation::AspectRatio(size);
			return result;
		}

		/**
		@brief	幅を基準に高さを自動計算
		@param	size		[in]	元のサイズ
		@param	width		[in]	変更後の幅
		@return	二次元サイズ
		*/
		inline D3DXVECTOR2 AdjustSizeByWidth(D3DXVECTOR2 size, float width)
		{
			D3DXVECTOR2 result = size;
			result.x = width;

			// 幅と比率から高さを計算
			result.y = width / UtilFunc::Calculation::AspectRatio(size);
			return result;
		}

		/**
		@brief	HSV値をRGB値に変換
		@details https://ja.wikipedia.org/wiki/HSV%E8%89%B2%E7%A9%BA%E9%96%93
		@param	H	[in]	色相
		@param	S	[in]	彩度
		@param	V	[in]	明度
		@return	RGB値
		*/
		inline D3DXCOLOR HSVtoRGB(float H, float S, float V)
		{
			if (H > 360.0f)
			{
				H -= 360.0f;
			}
			else if (H < 0.0f)
			{
				H += 360.0f;
			}
			float C = V * S;
			float X = C * (1 - (float)abs(fmod(H / 60.0f, 2) - 1));
			float m = V - C;

			// RGB別の数値
			float R = 0.0f, G = 0.0f, B = 0.0f;
			if (H >= 0.0f && H < 60.0f)
			{
				R = C;
				G = X;
				B = 0.0f;
			}
			else if (H >= 60.0f && H < 120.0f)
			{
				R = X;
				G = C;
				B = 0.0f;
			}
			else if (H >= 120.0f && H < 180.0f)
			{
				R = 0.0f;
				G = C;
				B = X;
			}
			else if (H >= 180.0f && H < 240.0f)
			{
				R = 0.0f;
				G = X;
				B = C;
			}
			else if (H >= 240.0f && H < 300.0f)
			{
				R = X;
				G = 0.0f;
				B = C;
			}
			else 
			{
				R = C;
				G = 0.0f;
				B = X;
			}

			return D3DXCOLOR(R + m, G + m, B + m, 1.0f);
		}

		/**
		@brief	ワイド文字列からマルチバイト文字列に変換する関数
		@param	wideStr		[in]	ワイド文字列
		@return	マルチバイト文字列
		*/
		inline std::string WideToMultiByte(const wchar_t* wideStr)
		{
			if (wideStr == nullptr)
				return std::string();

			// ワイド文字列の長さを取得
			int wideLength = wcslen(wideStr);

			// マルチバイト文字列の長さを計算
			int multiLength = WideCharToMultiByte(CP_UTF8, 0, wideStr, wideLength, nullptr, 0, nullptr, nullptr);

			if (multiLength == 0)
				return std::string();

			// マルチバイト文字列を格納するためのバッファを確保
			std::string multiString(multiLength, 0);

			// ワイド文字列をマルチバイト文字列に変換
			WideCharToMultiByte(CP_UTF8, 0, wideStr, wideLength, &multiString[0], multiLength, nullptr, nullptr);

			return multiString;
		}


		/**
		@brief	\\\\を\\に置換する
		@param	str		[in]	文字列
		@return	変換されたstring文字
		*/
		inline std::string ReplaceBackslash(std::string str)
		{
			size_t pos = 0;

			while ((pos = str.find("\\\\", pos)) != std::string::npos)
			{
				// \\\\を\\に置換
				str.replace(pos, 2, "\\");

				// \\の次から検索を再開
				pos += 1;
			}
			return str;
		}

		/**
		@brief	/を\\に置換する
		@param	str		[in]	文字列
		@return	変換されたstring文字
		*/
		inline std::string ReplaceForwardSlashes(std::string str)
		{
			for (size_t i = 0; i < str.size(); i++) 
			{
				if (str[i] == '/') 
				{
					str.replace(i, 1, "\\");
				}
			}
			return str;
		}
	}

}



#endif