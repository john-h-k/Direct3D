#pragma once
#pragma pack(push, 1)
#include <windows.h>

// Guaranteed to fit well in an array
typedef struct D3DXCOLOR
{
	FLOAT rgba[4];
} D3DXCOLOR;

inline FLOAT* GetFloatPtrFromColor(D3DXCOLOR& color)
{
	return +color.rgba;
}

inline FLOAT* GetFloatPtrFromColor(D3DXCOLOR* color)
{
	return +(*color).rgba;
}

#pragma pack(pop)