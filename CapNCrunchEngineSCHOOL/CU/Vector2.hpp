#pragma once
#include <cmath>

namespace CU
{
	template <class T> class Vector2
	{
	public:
		static const Vector2<T> Zero;

		T x;
		T y;

		Vector2<T>() : x(0), y(0) {}
		Vector2<T>(const T& aX, const T& aY) : x(aX), y(aY) {}
		Vector2<T>(const Vector2<T>& aVector) = default;
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;
		Vector2<T> operator-() const { return Vector2<T>(-x, -y); }
		bool operator==(const Vector2<T>& aVector)
		{
			return (x == aVector.x) && (y == aVector.y);
		}
		bool operator!=(const Vector2<T>& aVector) { return !(*this == aVector); }
		~Vector2<T>() = default;
		T LengthSqr() const { return x * x + y * y; }
		T Length() const { return sqrt(LengthSqr()); }
		Vector2<T> GetNormalized() const
		{
			if (x == 0 && y == 0)
				return Vector2<T>();
			return Vector2<T>(x / Length(), y / Length());
		}
		void Normalize()
		{
			if (x == 0 && y == 0)
				return;
			T length = Length();
			x /= length;
			y /= length;
		}
		T Dot(const Vector2<T>& aVector) const { return x * aVector.x + y * aVector.y; }
	};

	template <class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y);
	}
	template <class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1) { return aVector0 + (-aVector1); }
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}
	template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector) { return aVector * aScalar; }
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar) { return aVector * (T(1) / aScalar); }
	template <class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1) 
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
	}
	template <class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1) { aVector0 += (-aVector1); }
	template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar) 
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
	}
	template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar) { aVector *= (T(1) / aScalar); }

	typedef Vector2<float> Vector2f;

	template<typename T>
	const Vector2<T> Vector2<T>::Zero(0, 0, 0);
}