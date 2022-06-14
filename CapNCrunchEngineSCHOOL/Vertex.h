#pragma once
#include "CU/Vector2.hpp"
#include "CU/Vector3.hpp"

struct Vertex
{
	CU::Vector3f position;
	CU::Vector2f uv;
	CU::Vector3f normal;
	CU::Vector3f tangent;
	CU::Vector3f bitangent;
};