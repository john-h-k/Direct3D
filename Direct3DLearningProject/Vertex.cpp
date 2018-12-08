#include "Vertex.h"

Vertex::Vertex(const float x, const float y, const float z,
               const float red, const float green, const float blue, const float alpha)
{
	position = DirectX::XMFLOAT3(x, y, z);
	color = DirectX::XMFLOAT4(red, green, blue, alpha);
}