#include "ivypch.h"
#include "Framebuffer.h"

Ivy::Framebuffer::Framebuffer(uint32_t width, uint32_t height, uint32_t samples)
	: mWidth(width)
	, mHeight(height)
	, mSamples(samples)
{
	//Invalidate();
	Create(width, height);
}

void Ivy::Framebuffer::Create(uint32_t width, uint32_t height)
{
	glGenFramebuffers(1, &mDepthMapFBO);

	glCreateTextures(GL_TEXTURE_2D, 1, &mDepthTexture);
	glBindTexture(GL_TEXTURE_2D, mDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBO);
	glViewport(0, 0, mWidth, mHeight);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Debug::CoreError("Framebuffer is incomplete!");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void Ivy::Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBO);

}

void Ivy::Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Ivy::Framebuffer::Prepare()
{
	Bind();
	glViewport(0, 0, mWidth, mHeight);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthAttachment, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	Unbind();
}

void Ivy::Framebuffer::Invalidate()
{
	if(mID)
	{
		glDeleteFramebuffers(1, &mID);
		glDeleteTextures(1, &mColorAttachment);
		glDeleteTextures(1, &mDepthAttachment);
	}

	glCreateFramebuffers(1, &mID);
	glBindFramebuffer(GL_FRAMEBUFFER, mID);

	glCreateTextures(GL_TEXTURE_2D, 1, &mColorAttachment);
	glBindTexture(GL_TEXTURE_2D, mColorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorAttachment, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &mDepthAttachment);
	glBindTexture(GL_TEXTURE_2D, mDepthAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Debug::CoreError("Framebuffer is incomplete!");
	}

	//glDrawBuffers(1, GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Ivy::Framebuffer::Resize(uint32_t width, uint32_t height)
{
}
