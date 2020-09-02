#pragma once
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Ivy
{

	// Managed (Ref counting) pointers
	template<typename T>
	using Ptr = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ptr<T> CreatePtr(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

;

	// Vectors
	using Vec2 = glm::vec2;
	using Vec3 = glm::vec3;
	using Vec4 = glm::vec4;
	using VecI2 = glm::ivec2;
	using VecI3 = glm::ivec3;
	using VecI4 = glm::ivec4;

	// Matrices
	using Mat2 = glm::mat2;
	using Mat3 = glm::mat3;
	using Mat4 = glm::mat4;

	// Vertex
	struct Vertex
	{
		Vec3 position;
		Vec4 color;
		Vec2 texcoord;
	};

	// Strings
	using String = std::string;
	using StringStream = std::stringstream;

	// Datastructures
	template<typename T>
	using Vector = std::vector<T>;

	template<typename First, typename Second>
	using UnorderedMap = std::unordered_map<First, Second>;

}