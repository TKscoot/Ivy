#pragma once
#include <memory>
#include "ivypch.h"

namespace Ivy
{

	template<typename T>
	using Ptr = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ptr<T> CreatePtr(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	using Vec2 = glm::vec2;
	using Vec3 = glm::vec3;
	using Vec4 = glm::vec4;
	using VecI2 = glm::ivec2;
	using VecI3 = glm::ivec3;
	using VecI4 = glm::ivec4;

	using Mat2 = glm::mat2;
	using Mat3 = glm::mat3;
	using Mat4 = glm::mat4;

	using String = std::string;

	template<typename First, typename Second>
	using UnorderedMap = std::unordered_map<First, Second>;

}