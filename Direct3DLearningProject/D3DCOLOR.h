#pragma once
#pragma pack(push, 4)
#include <windows.h>

// Guaranteed to fit well in an array
typedef struct D3DXCOLOR
{
	FLOAT r;
	FLOAT g;
	FLOAT b;
	FLOAT a;
} D3DXCOLOR;

inline FLOAT* GetFloatPtrFromColor(D3DXCOLOR& color)
{
	return &color.r;
}

inline FLOAT* GetFloatPtrFromColor(D3DXCOLOR* color)
{
	return &(*color).r;
}

#pragma pack(pop)