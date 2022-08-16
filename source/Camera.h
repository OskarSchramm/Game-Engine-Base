#pragma once
#include "CU/Matrix4x4.hpp"
#include "CU/Vector2.hpp"

class Camera
{
public:
	Camera();
	~Camera();

	void Init(const CU::Vector3f& aPos, const CU::Vector3f& aRot, const float aFoV, const CU::Vector2f& aRes, const float aNearPlane, const float aFarPlane);
	void Update(const float aDT);

	void SetPosition(const CU::Vector3f& aPosition);
	void SetRotation(const CU::Vector3f& aRotation);
	void SetPerspectiveProjection(const float aHorizontalFoV, const CU::Vector2f& aResolution, const float aNearPlane, const float aFarPlane);

	inline CU::Matrix4x4f& GetViewMatrix() { return myViewMatrix; }
	inline CU::Matrix4x4f& GetProjectionMatrix() { return myProjectionMatrix; }
	inline CU::Matrix4x4f& GetTransform() { return myTransform; }

	//Left here
	CU::Matrix4x4f GetReflectionMatrix(const float h);
private:
	float myNearPlane;
	float myFarPlane;

	CU::Matrix4x4f myTransform;
	CU::Matrix4x4f myViewMatrix;
	CU::Matrix4x4f myProjectionMatrix;
	float myMoveSpeed;
	float myRotationSpeed;
};