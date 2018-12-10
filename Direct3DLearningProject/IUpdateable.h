#pragma once
namespace FactaLogicaSoftware
{
	class IUpdateable // NOLINT(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
	{
	public:
		virtual ~IUpdateable() = default;
		virtual void Update(double deltaMilliseconds = 0) = 0;
	};
}
