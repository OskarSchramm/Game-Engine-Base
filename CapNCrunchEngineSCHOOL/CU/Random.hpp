#pragma once
#include <random>

namespace CommonUtilities
{
	class Random
	{
	public:
		static void Init()
		{
			myRandomEngine.seed(std::random_device()());
		}

		static float Float()
		{
			return (float)myDistribution(myRandomEngine) / (float)(std::numeric_limits<uint32_t>::max)();
		}

		float GetRandomFloat(float aMin = -100000.0f, float aMax = 100000.0f);
		int GetRandomInt(int aMin = -100000, int aMax = 100000);
	private:
		static std::mt19937 myRandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> myDistribution;
	};
}