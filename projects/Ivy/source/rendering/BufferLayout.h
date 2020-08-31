#pragma once
#include <vector>
#include <glad/glad.h>
#include "environment/Log.h"

namespace Ivy
{
	// Code used from Yan Chernikov (TheCherno)

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;
		}

		Debug::CoreError("Specified unknown ShaderDataType!");
		return 0;
	}

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
		}

		Debug::CoreError("Specified unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		size_t Offset;
		bool Normalized;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Float:   return 1;
			case ShaderDataType::Float2:  return 2;
			case ShaderDataType::Float3:  return 3;
			case ShaderDataType::Float4:  return 4;
			case ShaderDataType::Mat3:    return 3; // 3* float3
			case ShaderDataType::Mat4:    return 4; // 4* float4
			case ShaderDataType::Int:     return 1;
			case ShaderDataType::Int2:    return 2;
			case ShaderDataType::Int3:    return 3;
			case ShaderDataType::Int4:    return 4;
			case ShaderDataType::Bool:    return 1;
			}

			Debug::CoreError("Specified unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetsAndStride()
		{
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};


	//enum class ShaderDataType
	//{
	//	None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	//};
	//
	//struct BufferElement
	//{
	//	ShaderDataType type;
	//	uint32_t count;
	//	bool	 normalized;
	//
	//	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	//	{
	//		switch (type)
	//		{
	//		case ShaderDataType::Float:    return 4;
	//		case ShaderDataType::Float2:   return 4 * 2;
	//		case ShaderDataType::Float3:   return 4 * 3;
	//		case ShaderDataType::Float4:   return 4 * 4;
	//		case ShaderDataType::Mat3:     return 4 * 3 * 3;
	//		case ShaderDataType::Mat4:     return 4 * 4 * 4;
	//		case ShaderDataType::Int:      return 4;
	//		case ShaderDataType::Int2:     return 4 * 2;
	//		case ShaderDataType::Int3:     return 4 * 3;
	//		case ShaderDataType::Int4:     return 4 * 4;
	//		case ShaderDataType::Bool:     return 1;
	//		}
	//
	//		Debug::CoreError("Specified unknown ShaderDataType!");
	//		return 0;
	//	}
	//};
	//
	//class BufferLayout
	//{
	//public:
	//	BufferLayout()
	//		:mStride(0)
	//	{
	//	}
	//
	//	template<typename T>
	//	void Push(int count, bool normalized = false)
	//	{
	//
	//	}
	//
	//	template<typename T>
	//	void Push<float>(int count, bool normalized)
	//	{
	//		mElements.push_back({ GL_FLOAT, count, normalized });
	//		mStride += sizeof(GLfloat);
	//	}
	//
	//	template<typename T>
	//	void Push<unsigned int>(int count, bool normalized)
	//	{
	//		mElements.push_back({ GL_UNSIGNED_INT, count, normalized });
	//		mStride += sizeof(GLuint);
	//
	//	}
	//
	//	inline uint32_t GetStride() const { return mStride; }
	//
	//	inline const std::vector<BufferElement> GetElements() const& { return mElements; }
	//
	//private:
	//	std::vector<BufferElement> mElements;
	//	uint32_t				   mStride = 0;
	//
	//};
}
