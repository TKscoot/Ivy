#pragma once
#include "Types.h"
#include "environment/Log.h"

namespace Ivy
{
	class Framebuffer
	{
	public:
		Framebuffer(uint32_t width, uint32_t height, uint32_t samples);

		void Create(uint32_t width, uint32_t height);

		void Bind();
		void Unbind();

		void Prepare();

		void Invalidate();

		void Resize(uint32_t width, uint32_t height); // TODO: implement

		uint32_t GetDepthAttachmentID() { return mDepthAttachment; }
		uint32_t GetColorAttachmentID() { return mColorAttachment; }

		uint32_t GetDepthTextureID() { return mDepthTexture; }
		uint32_t GetDepthFboID() { return mDepthMapFBO; }

	private:
		uint32_t mID = 0;
		uint32_t mDepthAttachment = 0, mColorAttachment = 0;

		uint32_t mDepthMapFBO  = 0;
		uint32_t mDepthTexture = 0;

		uint32_t mWidth = 0, mHeight = 0, mSamples = 0;

	};
}