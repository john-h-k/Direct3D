#pragma once

#include <DirectXMath.h>

struct Vertex
{
	Vertex() = default;
	Vertex(float x, float y, float z,
		float red, float green, float blue, float alpha);

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};
