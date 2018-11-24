#pragma once
class IUpdateable
{
public:
	virtual ~IUpdateable() = 0;
	virtual void Update() = 0;
};

inline IUpdateable::~IUpdateable()
= default;
