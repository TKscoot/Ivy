#pragma once
#include <Types.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Ivy
{
	// Float Vectors
	inline std::ostream& operator<<(std::ostream& os, const Vec2& c)
	{
		return os << "x:" << c.x << " y:" << c.y;
	}
	inline std::ostream& operator<<(std::ostream& os, const Vec3& c)
	{
		return os << "x:" << c.x << " y:" << c.y << " z:" << c.z;
	}	
	inline std::ostream& operator<<(std::ostream& os, const Vec4& c)
	{
		return os << "x:" << c.x << " y:" << c.y << " z:" << c.z << " w:" << c.w;
	}	

	// Int Vectors
	inline std::ostream& operator<<(std::ostream& os, const VecI2& c)
	{
		return os << "x:" << c.x << " y:" << c.y;
	}
	inline std::ostream& operator<<(std::ostream& os, const VecI3& c)
	{
		return os << "x:" << c.x << " y:" << c.y << " z:" << c.z;
	}
	inline std::ostream& operator<<(std::ostream& os, const VecI4& c)
	{
		return os << "x:" << c.x << " y:" << c.y << " z:" << c.z << " w:" << c.w;
	}
	
}