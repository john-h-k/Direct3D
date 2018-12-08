#pragma once

#include <DirectXMath.h>

struct Vertex final
{
	Vertex() = default;
	Vertex(float x, float y, float z,
		float red, float green, float blue, float alpha);

	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT4 color{};
};
