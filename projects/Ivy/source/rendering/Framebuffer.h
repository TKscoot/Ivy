#pragma once
#include "Types.h"
#include "environment/Log.h"

namespace Ivy
{
	class Framebuffer
	{
	public:
		/*!
		 * Framebuffer constructor
		 * Used to create a Framebuffer object to render to a texture
		 * 
		 * \param width Width of the texture
		 * \param height Height of the texture
		 */
		Framebuffer(uint32_t width, uint32_t height);

		/*!
		 * Used to create a Framebuffer object to render to a texture
		 *
		 * \param width Width of the texture
		 * \param height Height of the texture
		 */
		void Create(uint32_t width, uint32_t height);

		/*!
		 * Sets the Framebuffer as active target
		 * 
		 */
		void Bind();

		/*!
		 * Unbinds the Framebuffer and tells renderer to use default target
		 * 
		 */
		void Unbind();

		/*!
		 * Gets the depth texture id
		 * 
		 * \return OpenGL texture id
		 */
		uint32_t GetDepthTextureID() { return mDepthTexture; }

		/*!
		 * Gets the depth texture id
		 *
		 * \return OpenGL texture id
		 */
		uint32_t GetDepthFboID() { return mDepthMapFBO; }

	private:
		uint32_t mID = 0;

		uint32_t mDepthMapFBO  = 0;
		uint32_t mDepthTexture = 0;

		uint32_t mWidth = 0, mHeight = 0;

	};
}