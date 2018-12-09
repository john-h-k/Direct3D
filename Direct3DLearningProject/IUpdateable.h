#pragma once
class IUpdateable // NOLINT(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
{
public:
	virtual ~IUpdateable() = default;
	virtual void Update(double secs = 0) = 0;
};