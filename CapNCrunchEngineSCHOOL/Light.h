#pragma once
#include "CU/Vector3.hpp"
#include "CU/Vector4.hpp"

class Light
{
	struct DirectionalLight
	{
		CU::Vector3f myDirection;
		CU::Vector4f myColor;
	};

	struct AmbientLight
	{
		CU::Vector4f mySkyColor;
		CU::Vector4f myGroundColor;
	};

public:
	Light() = default;
	~Light() = default;

	inline void SetDirectionalLight(const CU::Vector3f& aDir, const CU::Vector4f& aColor)
	{
		myDirectionalLight.myDirection = aDir.GetNormalized();
		myDirectionalLight.myColor = aColor;
	};
	inline void SetAmbientLight(const CU::Vector4f& aSkyColor, const CU::Vector4f& aGroundColor)
	{
		myAmbientLight.mySkyColor = aSkyColor;
		myAmbientLight.myGroundColor = aGroundColor;
	};
	inline DirectionalLight GetDirectionalLight() const { return myDirectionalLight; }
	inline AmbientLight GetAmbientLight() const { return myAmbientLight; }
private:
	DirectionalLight myDirectionalLight;
	AmbientLight	 myAmbientLight;
};