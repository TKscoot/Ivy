#include "ivypch.h"
#include "TerrainMaterial.h"

Ivy::TerrainMaterial::TerrainMaterial(Ptr<Entity> entity) : Component::Component(entity)
{
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mMaxTextures);

	mTextures.resize(mMaxTextures);
	std::fill(mTextures.begin(), mTextures.end(), nullptr);
	InitDefaultShader();

}

Ivy::TerrainMaterial::~TerrainMaterial()
{
	for(int i = 0; i < mTextures.size(); i++)
	{
		mTextures[i]->Destroy();
	}
}

void Ivy::TerrainMaterial::AddTexture(int slot, Ptr<Texture2D> tex)
{
	if(slot > mMaxTextures || slot < 0)
	{
		Debug::CoreError("Exceeded max texture units! (max: {})", mMaxTextures);
		return;
	}
	else 
	{
		if(mTextures[slot] != nullptr)
		{
			Debug::CoreWarning("Replacing Texture: {}", mTextures[slot]->GetFilepath());
		}
		mTextures[slot] = tex;
	}
}

void Ivy::TerrainMaterial::BindTextures()
{
	for(int i = 0; i < mTextures.size(); i++)
	{
		if(mTextures[i])
		{
			mTextures[i]->Bind(i);
		}
	}

}

void Ivy::TerrainMaterial::InitDefaultShader()
{

	String vertexFilepath   = "shaders/debug/Wireframe_Terrain.vert";
	String fragmentFilepath = "shaders/debug/Terrain_Wireframe.frag";
	String geometryFilepath = "shaders/debug/Wireframe.geom";

	if(!std::filesystem::exists(vertexFilepath))
	{
		std::filesystem::path path{ vertexFilepath };
		std::filesystem::create_directories(path.parent_path());

		std::ofstream ofs(path);
		String src = R"(
#version 450 core

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec3 FragPos;
out vec3 ViewPosition;
out vec2 TexCoords;
out vec3 Normal;
out vec3 Halfway;

const vec3 light = normalize (vec3 (2, 1, 3));

void main()
{
    FragPos = vec3(model * vec4(aPosition.xyz, 1.0));   
    TexCoords = aTexCoord;
	ViewPosition = vec3(view * vec4(FragPos, 1.0));

	mat3 modelVector = transpose(inverse(mat3(model)));
    
	//Normal = normalize(modelVector * aNormal);
	Normal = mat3(model) * aNormal;
	Halfway = normalize(ViewPosition + light);

    gl_Position = projection * view * model * vec4(aPosition.xyz, 1.0);

}
		)";
		ofs << src;
		ofs.close();
	}
	if(!std::filesystem::exists(fragmentFilepath))
	{
		std::filesystem::path path{ fragmentFilepath };
		std::filesystem::create_directories(path.parent_path());

		std::ofstream ofs(path);
		String src = R"(
#version 450 core
 
out vec4 FragColor;

in vec3 FragPos;
in vec3 ViewPosition;
in vec2 TexCoords;
in vec3 Normal;
in vec3 Halfway;


const vec3 Ca = vec3(  0.005, 0.00492, 0.00439);
const vec3 Cd = vec3(  1.0,  0.0, 0.0);
const vec3 Cs = vec3 (.8, .8, .8);
const vec3 Ce = vec3 ( 0,  0,  0);

// Direktionales Licht (im Unendlichen)
const vec3 light = normalize (vec3 (2, 1, 3));

void main()
{        
	vec3 l = vec3(-2., 4., 1.);
	vec3 n = normalize(Normal);
	vec3 unpackedN = 2.0 * n - 1.0;
	float Id = max(dot(light, n), 0.0f);
	float Is = pow(max(dot(normalize(Halfway), n), 0), 30);
	vec3 color = Ca + Ce + Cd * Id;// + Cs * Is;

	// Gamma correction
    float gamma = 2.2;
    color.rgb = pow(color.rgb, vec3(1.0/gamma));

    FragColor = vec4(vec3(Id), 1.0);
}
		)";
		ofs << src;
		ofs.close();
	}

	mShader = CreatePtr<Shader>(vertexFilepath, fragmentFilepath, geometryFilepath);

}

void Ivy::TerrainMaterial::UploadUniforms()
{
	for(auto& ui : mUniforms)
	{
		if(ui.type == INT)
			mShader->SetUniformInt(ui.name, *std::get<int*>(ui.value));

		if(ui.type == FLOAT)
			mShader->SetUniformFloat(ui.name, *std::get<float*>(ui.value));

		if(ui.type == VEC2)
			mShader->SetUniformFloat2(ui.name, *std::get<Vec2*>(ui.value));

		if(ui.type == VEC3)
			mShader->SetUniformFloat3(ui.name, *std::get<Vec3*>(ui.value));

		if(ui.type == VEC4)
			mShader->SetUniformFloat4(ui.name, *std::get<Vec4*>(ui.value));

		if(ui.type == MAT3)
			mShader->SetUniformMat3(ui.name, *std::get<Mat3*>(ui.value));

		if(ui.type == MAT4)
			mShader->SetUniformMat4(ui.name, *std::get<Mat4*>(ui.value));
	}

	mShader->SetUniformFloat3("dirLightDirection", mEntity->GetAttachedScene()->GetDirLight().direction);
}
