#pragma once
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Quaternion.h"
#include <cstring>
#include <cassert>
#include <cmath>

namespace CU
{
	template <typename T>
	class Matrix4x4
	{
	public:
		Matrix4x4<T>() : myArray{ {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} } {}
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix)
		{
			std::memcpy(myArray, aMatrix.myArray, sizeof(myArray));
		}

		Vector3<T> GetPosition() const
		{
			return { (*this)(4,1), (*this)(4,2), (*this)(4,3) };
		}
		Vector4<T> GetPositionVec4() const
		{
			return { (*this)(4,1), (*this)(4,2), (*this)(4,3), (*this)(4,4) };
		}
		void SetPositionRelative(const Vector3<T>& aVector3)
		{
			myRows[3].x = aVector3.x;
			myRows[3].y = aVector3.y;
			myRows[3].z = aVector3.z;
		}
		void SetScale(const Vector3<T>& aVector3)
		{
			myRows[0].x = aVector3.x;
			myRows[1].y = aVector3.y;
			myRows[2].z = aVector3.z;
		}
		void SetScale(const float aScale)
		{
			myRows[0].x = aScale;
			myRows[1].y = aScale;
			myRows[2].z = aScale;
		}

		static Matrix4x4<T> CreateTranslationMatrix(const Vector3<T>& aVector3)
		{
			Matrix4x4<T> result;

			result.myRows[3].x = aVector3.x;
			result.myRows[3].y = aVector3.y;
			result.myRows[3].z = aVector3.z;

			return result;
		}
		void SetRotation(const Vector3<T>& aVector3)
		{
			ResetRotation();
			*this *= CreateRollPitchYawMatrix(aVector3);
		}
		inline void ResetRotation()
		{
			myArray[1][1] = 1;
			myArray[1][2] = 0;
			myArray[2][1] = 0;
			myArray[2][2] = 1;
			myArray[0][0] = 1;
			myArray[0][2] = 0;
			myArray[2][0] = 0;
			myArray[0][1] = 0;
			myArray[1][0] = 0;
		}
		inline Matrix4x4<T> CreateRollPitchYawMatrix(Vector3<T> aPitchYawRollvector)
		{
			//pitch * roll * yaw

			const float radConst = 3.141f / 180.f;
			aPitchYawRollvector *= radConst;

			Quaternion<T> rotation = Quaternion<T>(aPitchYawRollvector);
			Vector4<T> values = { rotation.w, rotation.x, rotation.y, rotation.z };
			return CreateRotationMatrixFromQuaternionVectorWXYZ(values);
		}
		inline Matrix4x4<T> CreateRotationMatrixFromQuaternionVectorWXYZ(Vector4<T> aVector)
		{
			Matrix4x4<T> result;

			T qxx(aVector.y * aVector.y);
			T qyy(aVector.z * aVector.z);
			T qzz(aVector.w * aVector.w);

			T qxz(aVector.y * aVector.w);
			T qxy(aVector.y * aVector.z);
			T qyz(aVector.z * aVector.w);

			T qwx(aVector.x * aVector.y);
			T qwy(aVector.x * aVector.z);
			T qwz(aVector.x * aVector.w);

			result.myRows[0].x = T(1) - T(2) * (qyy + qzz);
			result.myRows[0].y = T(2) * (qxy + qwz);
			result.myRows[0].z = T(2) * (qxz - qwy);

			result.myRows[1].x = T(2) * (qxy - qwz);
			result.myRows[1].y = T(1) - T(2) * (qxx + qzz);
			result.myRows[1].z = T(2) * (qyz + qwx);

			result.myRows[2].x = T(2) * (qxz + qwy);
			result.myRows[2].y = T(2) * (qyz - qwx);
			result.myRows[2].z = T(1) - T(2) * (qxx + qyy);

			result.myArray[0][3] = result.myArray[1][3] = result.myArray[2][3] = 0;
			result.myArray[3][3] = 1;
			return result;
		}

		static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians)
		{
			Matrix4x4<T> m;
			m(2, 2) = std::cos(aAngleInRadians);
			m(2, 3) = std::sin(aAngleInRadians);
			m(3, 2) = -std::sin(aAngleInRadians);
			m(3, 3) = std::cos(aAngleInRadians);
			return m;
		}
		static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians)
		{
			Matrix4x4<T> m;
			m(1, 1) = std::cos(aAngleInRadians);
			m(1, 3) = -std::sin(aAngleInRadians);
			m(3, 1) = std::sin(aAngleInRadians);
			m(3, 3) = std::cos(aAngleInRadians);
			return m;
		}
		static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians)
		{
			Matrix4x4<T> m;
			m(1, 1) = std::cos(aAngleInRadians);
			m(1, 2) = std::sin(aAngleInRadians);
			m(2, 1) = -std::sin(aAngleInRadians);
			m(2, 2) = std::cos(aAngleInRadians);
			return m;
		}

		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose)
		{
			Matrix4x4 mt;
			for (size_t row = 0; row < 4; ++row)
			{
				for (size_t col = 0; col < 4; ++col)
					mt.myArray[row][col] = aMatrixToTranspose.myArray[col][row];
			}
			return mt;
		}
		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform)
		{
			Matrix4x4<T> inv = aTransform;

			auto t1 = aTransform.myRows[0].Dot(aTransform.myRows[3]);
			auto t2 = aTransform.myRows[1].Dot(aTransform.myRows[3]);
			auto t3 = aTransform.myRows[2].Dot(aTransform.myRows[3]);

			inv.myRows[3] = { 0,0,0,1 };

			inv = Transpose(inv);
			inv.myRows[3] = { -t1, -t2, -t3, 1 };
			return inv;
		}

		Matrix4x4<T>& operator=(const Matrix4x4<T>& aMatrix)
		{
			std::memcpy(myArray, aMatrix.myArray, sizeof(myArray));
			return *this;
		}
		T& operator()(const int aRow, const int aColumn)
		{
			ValidateIndices(aRow, aColumn);
			return myArray[aRow - 1][aColumn - 1];
		}
		const T& operator()(const int aRow, const int aColumn) const
		{
			ValidateIndices(aRow, aColumn);
			return myArray[aRow - 1][aColumn - 1];
		}
		bool operator!=(const Matrix4x4<T>& aMatrix) const
		{
			return std::memcmp(myArray, aMatrix.myArray, sizeof(myArray)); // should be faster than a for loop
		}
		bool operator==(const Matrix4x4<T>& aMatrix) const
		{
			return !(*this != aMatrix);
		}
		Matrix4x4<T>& operator+=(const Matrix4x4<T>& aMatrix)
		{
			for (size_t row = 0; row < 4; ++row)
				myRows[row] += aMatrix.myRows[row];
			return *this;
		}
		friend Matrix4x4<T> operator+(const Matrix4x4<T>& aLhs, const Matrix4x4<T>& aRhs)
		{
			Matrix4x4<T> m(aLhs);
			return m += aRhs;
		}
		Matrix4x4<T>& operator-=(const Matrix4x4<T>& aMatrix)
		{
			for (size_t row = 0; row < 4; ++row)
				myRows[row] -= aMatrix.myRows[row];
			return *this;
		}
		friend Matrix4x4<T> operator-(const Matrix4x4<T>& aLhs, const Matrix4x4<T>& aRhs)
		{
			Matrix4x4<T> m(aLhs);
			return m -= aRhs;
		}
		friend Matrix4x4<T> operator*(const Matrix4x4<T>& aLhs, const Matrix4x4<T>& aRhs)
		{
			Matrix4x4<T> m;
			for (size_t row = 0; row < 4; ++row)
				m.myRows[row] = aLhs.myRows[row] * aRhs;
			return m;
		}
		friend Vector4<T> operator*<>(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix);
		Matrix4x4<T>& operator*=(const Matrix4x4<T>& aMatrix)
		{
			*this = *this * aMatrix;
			return *this;
		}

	private:
		union
		{
			T myArray[4][4];
			Vector4<T> myRows[4];
		};

		void ValidateIndices(const int aRow, const int aColumn) const
		{
			assert(1 <= aRow && aRow <= 4 && 1 <= aColumn && aColumn <= 4 && "Matrix indices out of bounds");
		}
	};

	template <typename T>
	Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		return aVector.x * aMatrix.myRows[0] + aVector.y * aMatrix.myRows[1] + aVector.z * aMatrix.myRows[2] + aVector.w * aMatrix.myRows[3];
	}

	typedef Matrix4x4<float> Matrix4x4f;
}