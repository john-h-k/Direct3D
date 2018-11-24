#pragma once

#include <DirectXMath.h>

struct Vertex
{
	Vertex() = default;
	Vertex(float x, float y, float z);
	DirectX::XMFLOAT3 position;
};
