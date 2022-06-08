	#pragma once
#include "Camera.h"
#include <Windows.h>

Camera::Camera() : myMoveSpeed(3), myRotationSpeed(3) {}
Camera::~Camera() {}

void Camera::Init(const CU::Vector3f& aPos, const CU::Vector3f& aRot, 
	const float aFoV, const CU::Vector2f& aRes, const float aNearPlane, const float aFarPlane)
{
	myTransform.SetPositionRelative(aPos);
	myTransform.SetRotation(aRot);
	SetPerspectiveProjection(aFoV, aRes, aNearPlane, aFarPlane);
}

void Camera::SetPerspectiveProjection(const float aHorizontalFoV, const CU::Vector2f& aResolution, 
	const float aNearPlane, const float aFarPlane)
{
	myProjectionMatrix = {};

	myNearPlane = aNearPlane;
	myFarPlane  = aFarPlane;

	const float hFovRad = aHorizontalFoV * (3.1415f / 180.0f);
	
	const float vFovRad = 2 * std::atan(std::tan(hFovRad / 2.0f) * (aResolution.y / aResolution.x));

	float xScale = 1.0f / std::tanf(hFovRad / 2.0f);
	float yScale = 1.0f / std::tanf(vFovRad / 2.0f);
	float q = myFarPlane / (myFarPlane - myNearPlane);

	myProjectionMatrix(1, 1) = xScale;
	myProjectionMatrix(2, 2) = yScale;
	myProjectionMatrix(3, 3) = q;
	myProjectionMatrix(3, 4) = 1.0f / q;
	myProjectionMatrix(4, 3) = -q * myNearPlane;
	myProjectionMatrix(4, 4) = 0.0f;
}

void Camera::Update(const float aDT)
{
	CU::Vector3f nextPos = {};
	CU::Vector3f nextRot = {};


	if (GetAsyncKeyState('W'))
		nextPos.z += myMoveSpeed * aDT;
	if (GetAsyncKeyState('A'))
		nextPos.x -= myMoveSpeed * aDT;
	if (GetAsyncKeyState('S'))
		nextPos.z -= myMoveSpeed * aDT;
	if (GetAsyncKeyState('D'))
		nextPos.x += myMoveSpeed * aDT;

	if (GetAsyncKeyState('Q'))
		nextRot.y -= myRotationSpeed * aDT;
	if (GetAsyncKeyState('E'))
		nextRot.y += myRotationSpeed * aDT;
	if (GetAsyncKeyState('R'))
		nextRot.x -= myRotationSpeed * aDT;
	if (GetAsyncKeyState('T'))
		nextRot.x += myRotationSpeed * aDT;

	SetPosition(nextPos);
	SetRotation(nextRot);
	myViewMatrix = CU::Matrix4x4f::GetFastInverse(myTransform);
}

void Camera::SetPosition(const CU::Vector3f& aPosition)
{
	myTransform = CU::Matrix4x4f::CreateTranslationMatrix(aPosition) * myTransform;
}

void Camera::SetRotation(const CU::Vector3f& aRotation)
{
	myTransform = CU::Matrix4x4f::CreateRotationAroundX(aRotation.x) * myTransform;
	myTransform = CU::Matrix4x4f::CreateRotationAroundY(aRotation.y) * myTransform;
	myTransform = CU::Matrix4x4f::CreateRotationAroundZ(aRotation.z) * myTransform;
}