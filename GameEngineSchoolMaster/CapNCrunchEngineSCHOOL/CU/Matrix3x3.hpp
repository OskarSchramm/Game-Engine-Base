#pragma once
#include "Vector3.hpp"
#include "Matrix4x4.hpp"
#include <cstring>
#include <cassert>
#include <cmath>

namespace CU
{
	template <typename T>
	class Matrix3x3
	{
	public:
		Matrix3x3<T>() : myArray{ {1,0,0}, {0,1,0}, {0,0,1} } {}
		Matrix3x3<T>(const Matrix3x3<T>& aMatrix)
		{
			std::memcpy(myArray, aMatrix.myArray, sizeof(myArray));
		}
		Matrix3x3<T>& operator=(const Matrix3x3<T>& aMatrix)
		{
			std::memcpy(myArray, aMatrix.myArray, sizeof(myArray));
			return *this;
		}

		Matrix3x3<T>(const Matrix4x4<T>& aMatrix)
		{
			for (size_t row = 0; row < 3; ++row)
			{
				for (size_t col = 0; col < 3; ++col)
					myArray[row][col] = aMatrix(row + 1, col + 1);
			}
		}

		T& operator()(const int aRow, const int aColumn)
		{
			ValidateIndices(aRow, aColumn);
			return myArray[aRow-1][aColumn-1];
		}
		const T& operator()(const int aRow, const int aColumn) const
		{
			ValidateIndices(aRow, aColumn);
			return myArray[aRow-1][aColumn-1];
		}

		bool operator!=(const Matrix3x3<T>& aMatrix) const
		{
			return std::memcmp(myArray, aMatrix.myArray, sizeof(myArray)); // should be faster than a for loop
		}
		bool operator==(const Matrix3x3<T>& aMatrix) const
		{
			return !(*this != aMatrix);
		}

		Matrix3x3<T>& operator+=(const Matrix3x3<T>& aMatrix)
		{
			for (size_t row = 0; row < 3; ++row)
				myRows[row] += aMatrix.myRows[row];
			return *this;
		}
		friend Matrix3x3<T> operator+(const Matrix3x3<T>& aLhs, const Matrix3x3<T>& aRhs)
		{
			Matrix3x3<T> m(aLhs);
			return m += aRhs;
		}

		Matrix3x3<T>& operator-=(const Matrix3x3<T>& aMatrix)
		{
			for (size_t row = 0; row < 3; ++row)
				myRows[row] -= aMatrix.myRows[row];
			return *this;
		}
		friend Matrix3x3<T> operator-(const Matrix3x3<T>& aLhs, const Matrix3x3<T>& aRhs)
		{
			Matrix3x3<T> m(aLhs);
			return m -= aRhs;
		}

		friend Matrix3x3<T> operator*(const Matrix3x3<T>& aLhs, const Matrix3x3<T>& aRhs)
		{
			Matrix3x3<T> m;
			for (size_t row = 0; row < 3; ++row)
				m.myRows[row] = aLhs.myRows[row] * aRhs;
			return m;
		}

		friend Vector3<T> operator*<>(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix);

		Matrix3x3<T>& operator*=(const Matrix3x3<T>& aMatrix)
		{
			*this = *this * aMatrix;
			return *this;
		}

		static Matrix3x3<T> CreateRotationAroundX(T aAngleInRadians)
		{
			Matrix3x3<T> m;
			m(2, 2) = std::cos(aAngleInRadians);
			m(2, 3) = std::sin(aAngleInRadians);
			m(3, 2) = -std::sin(aAngleInRadians);
			m(3, 3) = std::cos(aAngleInRadians);
			return m;
		}
		static Matrix3x3<T> CreateRotationAroundY(T aAngleInRadians)
		{
			Matrix3x3<T> m;
			m(1, 1) = std::cos(aAngleInRadians);
			m(1, 3) = -std::sin(aAngleInRadians);
			m(3, 1) = std::sin(aAngleInRadians);
			m(3, 3) = std::cos(aAngleInRadians);
			return m;
		}
		static Matrix3x3<T> CreateRotationAroundZ(T aAngleInRadians)
		{
			Matrix3x3<T> m;
			m(1, 1) = std::cos(aAngleInRadians);
			m(1, 2) = std::sin(aAngleInRadians);
			m(2, 1) = -std::sin(aAngleInRadians);
			m(2, 2) = std::cos(aAngleInRadians);
			return m;
		}

		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose)
		{
			Matrix3x3 mt;
			for (size_t row = 0; row < 3; ++row)
			{
				for (size_t col = 0; col < 3; ++col)
					mt.myArray[row][col] = aMatrixToTranspose.myArray[col][row];
			}
			return mt;
		}

	private:
		union
		{
			T myArray[3][3];
			Vector3<T> myRows[3];
		};

		void ValidateIndices(const int aRow, const int aColumn) const
		{
			assert(1 <= aRow && aRow <= 3 && 1 <= aColumn && aColumn <= 3 && "Matrix indices out of bounds");
		}
	};

	template <typename T>
	Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
	{
		return aVector.x * aMatrix.myRows[0] + aVector.y * aMatrix.myRows[1] + aVector.z * aMatrix.myRows[2];
	}
}