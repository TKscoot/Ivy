#pragma once
#include "Types.h"
#include <variant>
#include "Component.h"
#include "scene/Entity.h"
#include "scene/Scene.h"
#include "rendering/Shader.h"
#include "rendering/Texture.h"
#include "core/Timer.h"

namespace Ivy
{
	class TerrainMaterial : public Component
	{
	public:
		enum UniformType
		{
			INT,
			FLOAT,
			VEC2,
			VEC3,
			VEC4,
			MAT3,
			MAT4
		};

		struct UniformInfo
		{
			String name;
			UniformType type;
			std::variant<int*, float*, Vec2*, Vec3*, Vec4*, Mat3*, Mat4*> value;
		};

		TerrainMaterial(Ptr<Entity> entity);
		~TerrainMaterial();

		void AddTexture(int slot, Ptr<Texture2D> tex);

		void SetShader(Ptr<Shader> shader) { mShader = shader; }
		Ptr<Shader> GetShader() { return mShader; }

		template <typename T>
		void AddUniform(String name, T* value)
		{
			std::type_index const index = std::type_index(typeid(T*));

			UniformInfo ui = {};
			ui.name = name; 
			ui.value = value;
			
			if(index == std::type_index(typeid(int*)))
			{
				ui.type = INT;		
			}
			else if(index == std::type_index(typeid(float*)))
			{
				ui.type = FLOAT;
			}
			else if(index == std::type_index(typeid(Vec2*)))
			{
				ui.type = VEC2;
			}
			else if(index == std::type_index(typeid(Vec3*)))
			{
				ui.type = VEC3;
			}
			else if(index == std::type_index(typeid(Vec4*)))
			{
				ui.type = VEC4;
			}
			else if(index == std::type_index(typeid(Mat3*)))
			{
				ui.type = MAT3;
			}
			else if(index == std::type_index(typeid(Mat4*)))
			{
				ui.type = MAT4;
			}
			else 
			{
				Debug::CoreError("No valid uniform type!");
				return;
			}

			mUniforms.push_back(ui);
		}

		void UploadUniforms();
		void BindTextures();
	
	private:
		void InitDefaultShader();

		Vector<Ptr<Texture2D>> mTextures;
		Vector<UniformInfo> mUniforms;

		int			   mMaxTextures = 0;

		Ptr<Shader>			   mShader = nullptr;

		Vec2				   mTextureTiling = Vec2(32.0f);


	};
}