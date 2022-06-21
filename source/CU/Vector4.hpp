#pragma once
#include <cmath>

namespace CU
{
	template <class T> class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;

		Vector4<T>() : x(0), y(0), z(0), w(0) {}
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW) : x(aX), y(aY), z(aZ), w(aW) {}
		Vector4<T>(const Vector4<T>& aVector) = default;
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;
		Vector4<T> operator-() const { return Vector4<T>(-x, -y, -z, -w); }
		bool operator==(const Vector4<T>& aVector)
		{
			return (x == aVector.x) && (y == aVector.y) && (z == aVector.z) && (w == aVector.w);
		}
		bool operator!=(const Vector4<T>& aVector) { return !(*this == aVector); }
		~Vector4<T>() = default;
		T LengthSqr() const { return x * x + y * y + z * z + w * w; }
		T Length() const { return sqrt(LengthSqr()); }
		Vector4<T> GetNormalized() const
		{
			if (x == 0 && y == 0 && z == 0 && w == 0)
				return Vector4<T>();
			return Vector4<T>(x / Length(), y / Length(), z / Length(), w / Length());
		}
		void Normalize()
		{
			if (x == 0 && y == 0 && z == 0 && w == 0)
				return;
			T length = Length();
			x /= length;
			y /= length;
			z /= length;
			w /= length;
		}
		T Dot(const Vector4<T>& aVector) const { return x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w; }
	};

	template <class T> Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z, aVector0.w + aVector1.w);
	}
	template <class T> Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1) { return aVector0 + (-aVector1); }
	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}
	template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector) { return aVector * aScalar; }
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar) { return aVector * (T(1) / aScalar); }
	template <class T> void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1) 
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
		aVector0.w += aVector1.w;
	}
	template <class T> void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1) { aVector0 += (-aVector1); }
	template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar) 
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
		aVector.w *= aScalar;
	}
	template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar) { aVector *= (T(1) / aScalar); }

	typedef Vector4<float> Vector4f;
}