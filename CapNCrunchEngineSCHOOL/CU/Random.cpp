#include "Random.hpp"
#include "pch.h"
#include "framework.h"

namespace CommonUtilities
{
	std::mt19937 Random::myRandomEngine;
	std::uniform_int_distribution<std::mt19937::result_type> Random::myDistribution;

	float Random::GetRandomFloat(float aMin, float aMax)
	{
		std::mt19937 rEngine(6);
		std::uniform_real_distribution<float> rnd(aMin, aMax);
		return rnd(rEngine);
	}

	int Random::GetRandomInt(int aMin, int aMax)
	{
		std::mt19937 rEngine(6);
		std::uniform_int_distribution<int> rnd(aMin, aMax);
		return rnd(rEngine);
	}
}