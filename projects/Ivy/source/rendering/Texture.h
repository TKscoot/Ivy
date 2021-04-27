#pragma once
#include <glad/glad.h>
#include <map>
#include "Types.h"
#include "environment/Log.h"
#include "rendering/Shader.h"

namespace Ivy
{
	class Texture2D
	{
	public:
		/*!
		 * Texture2D constructor
		 * Creates 2D texture object
		 * 
		 * \param width width of the texture in pixels
		 * \param height height of the texture in pixels
		 */
		Texture2D(uint32_t width, uint32_t height);

		/*!
		* Texture2D constructor
		* Creates 2D texture object
		*
		* \param width width of the texture in pixels
		* \param height height of the texture in pixels
		* \param data the color data of the texture
		*/
		Texture2D(uint32_t width, uint32_t height, void* data);

		/*!
		 * Texture2D constructor
		 * Creates and loads 2D texture object from a file
		 * 
		 * 
		 * \param filepath the full path and filename of the texture to load
		 */
		Texture2D(String filepath);		


		/*!
		 * Texture2D constructor
		 * Creates and loads 2D texture object from a file
		 *
		 *
		 * \param filepath the full path and filename of the texture to load
		 */
		Texture2D(String filepath, GLenum internalFormat, GLenum dataFormat);

		~Texture2D();

		/*!
		 * Loads a 2D texture from a file
		 * almost all normal filetypes (.png, .jpg, .bmp) are supported
		 * 
		 * \param filepath the full path and filename of the texture to load
		 */
		void Load(String filepath);

		/*!
		 * Loads a 2D texture from a file
		 * almost all normal filetypes (.png, .jpg, .bmp) are supported
		 *
		 * \param filepath the full path and filename of the texture to load
		 */
		void Load(String filepath, GLenum internalFormat, GLenum dataFormat);


		/*!
		 * Sets the pixeldata of the texture
		 * 
		 * \param data pointer to the data
		 * \param size size in bytes of the data
		 */
		void SetData(void* data, uint32_t size);

		/*!
		 * Binds the texture to the shader
		 * 
		 * \param slot the binding
		 * 
		 * shader syntax to access texture with the specified slot:
		 * layout(binding = <slot>) uniform <texture name>;
		 */
		void Bind(uint32_t slot = 0);

		/*!
		 * Gets the width in pixels of the texture
		 * 
		 * \return returns uint32_t pixel width
		 */
		uint32_t GetWidth()  const { return mWidth; }

		/*!
		 * Gets the height in pixels of the texture
		 *
		 * \return returns uint32_t pixel height
		 */
		uint32_t GetHeight() const { return mHeight; }

		/*!
		 * Gets the OpenGL ID of the texture object
		 *
		 * (Only use this if you know what you are doing!!!)
		 *
		 * \return returns uint32_t of the ID
		 */
		GLuint GetRendererID() const { return mID; }

		// TODO: void SetPixel(Vec2i coord, Vec4 colorValues); 

		static Texture2DData LoadTextureData(String file);

		String GetFilepath() { return mFilepath; }

		void Destroy();
	private:
		String	 mFilepath = "";

		GLuint	 mID			 = 0;
		GLenum	 mInternalFormat;
		GLenum	 mDataFormat;
		uint32_t mWidth          = 0;
		uint32_t mHeight         = 0;
	};

	class TextureCube
	{
	public:
		/*!
		 * TextureCube constructor
		 * Creates cube texture object
		 *
		 * \param filenames Vector of files to use as Cube texture
		 */
		TextureCube(Vector<String> filenames);
		~TextureCube();

		/*!
		 * TextureCube constructor
		 * Creates cube texture object
		 *
		 * \param filenames Vector of files to use as Cube texture
		 */
		TextureCube(GLenum format, uint32_t width, uint32_t height);
		
		/*!
		 * TextureCube constructor
		 * Creates cube texture object
		 *
		 * \param Paths to images for the cubetexture
		 */
		TextureCube(String right,
				    String left,
				    String top,
				    String bottom,
				    String back,
				    String front);

		/*!
		 * Loads cube texture with specified images
		 *
		 * \param filenames Vector of files to use as Cube texture
		 */
		void Load(Vector<String> filenames);

		void Destroy();

		/*!
		 * Loads cube texture with specified images
		 *
		 * \param filenames Vector of files to use as Cube texture
		 */
		void Load(String right,
				  String left,
				  String top,
				  String bottom,
				  String back,
				  String front);

		/*!
		 * Binds the cube texture to the shader
		 * 
		 * \param slot Which slot it should be bound to shader. Default = 0
		 */
		void Bind(uint32_t slot = 0);

		/*!
		 * Gets the renderer ID
		 * \return Opengl Renderobject ID
		 * 
		 */
		GLuint GetID() { return mID; }


		/*!
		 * Gets the width in pixels
		 * 
		 * \return width of the texture in pixels
		 */
		uint32_t GetWidth() { return mWidth; }

		/*!
		 * Gets the width in pixels
		 *
		 * \return width of the texture in pixels
		 */
		uint32_t GetHeight() { return mHeight; }

		/*!
		 * Calculates the count of mip levels
		 * 
		 * \return count of mip levels
		 */
		uint32_t GetMipLevelCount()
		{
			uint32_t levels = 1;
			while((mWidth | mHeight) >> levels)
				levels++;

			return levels;
		}



	private:
		GLuint	 mID = 0;

		uint32_t mWidth  = 0;
		uint32_t mHeight = 0;

	};

	class TextureHDRI
	{
	public:
		/*!
		* HdriTextureCube constructor
		* Creates cube texture object
		*
		* \param filename HDRI file
		*/
		TextureHDRI(String filename);
		~TextureHDRI();

		void Destroy();

		/*!
		 * Loads cube texture with specified images
		 *
		 * \param filenames Vector of files to use as Cube texture
		 */
		void Load(String filename);

		/*!
		 * Binds the cube texture to the shader
		 *
		 * \param slot Which slot it should be bound to shader. Default = 0
		 */
		void Bind(uint32_t slot = 0);

		GLuint GetID() { return mID; }

		void ConvertToCubemap(Ptr<TextureCube> outputTexCube);
	private:
		GLuint	 mID = 0;

	};

}