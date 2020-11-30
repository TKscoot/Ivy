#pragma once
#include "Types.h"
#include "environment/Log.h"

namespace Ivy
{
	class Framebuffer
	{
	public:
		Framebuffer(uint32_t width, uint32_t height);

		void Create(uint32_t width, uint32_t height);

		void Bind();
		void Unbind();

		uint32_t GetDepthTextureID() { return mDepthTexture; }
		uint32_t GetDepthFboID() { return mDepthMapFBO; }

	private:
		uint32_t mID = 0;

		uint32_t mDepthMapFBO  = 0;
		uint32_t mDepthTexture = 0;

		uint32_t mWidth = 0, mHeight = 0;

	};
}