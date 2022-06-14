#pragma once
#include <cassert>

namespace CU
{
	constexpr float Pi = 3.14159265358979323846f;
	constexpr float Pi_Half = Pi / 2.f;
	constexpr float DegToRad = Pi / 180.f;
	constexpr float RadToDeg = 180.f / Pi;

	constexpr float Tau = Pi * 2.0f;
	constexpr double PiD = 3.1415926535897932384626433832795;
	constexpr double PiHalfD = PiD * 0.5;
	constexpr double TauD = PiD * 2.0;

	template<typename T>
	T Min(const T& aFirstValue, const T& aSecondValue)
	{
		return (aFirstValue < aSecondValue) ? aFirstValue : aSecondValue;
	}

	template<typename T>
	T Max(const T& aFirstValue, const T& aSecondValue)
	{
		return (aFirstValue > aSecondValue) ? aFirstValue : aSecondValue;
	}

	template<typename T>
	T Abs(T& aValue)
	{
		return (aValue < 0) ? aValue *= -1.0f : aValue;
	}

	template<typename T>
	T Clamp(const T& aMin, const T& aMax, const T& aValue)
	{
		assert(aMin <= aMax && aMax >= aMin && L"Fail when checking: Min: aMax , Max: aMin");

		if (aValue < aMin)
			return aMin;
		else if (aValue > aMax)
			return aMax;
		else
			return aValue;
	}

	template<typename T>
	T Lerp(const T& aFirstValue, const T& aSecondValue, const float somePercent)
	{
		assert(somePercent >= .0f && "Percent is less than 0");

		return (aFirstValue + somePercent * (aSecondValue - aFirstValue));
	}

	template<typename T>
	void Swap(T& aFirstValue, T& aSecondValue)
	{
		T temp = aFirstValue;
		aFirstValue = aSecondValue;
		aSecondValue = temp;
	}
}