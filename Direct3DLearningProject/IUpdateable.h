#pragma once
class IUpdateable
{
public:
	virtual ~IUpdateable() = default;
	virtual void Update() = 0;
};