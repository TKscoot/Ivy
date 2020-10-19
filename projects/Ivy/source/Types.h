#pragma once
#include <inttypes.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <set>
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
		Vec4 position;
		Vec3 normal;
        Vec3 tangent;
		Vec3 bitangent;
		Vec2 texcoord;
	};

	// Strings
	using String = std::string;
	using StringStream = std::stringstream;

	// IDs
	using ResourceID = std::uint32_t;

	// Datastructures
	template<typename T>
	using Vector = std::vector<T>;

	template<typename First, typename Second>
	using UnorderedMap = std::unordered_map<First, Second>;

	template<typename First, typename Second>
	using Set = std::set<First, Second>;

	template<typename First, typename Second>
	using Pair = std::pair<First, Second>;

}
