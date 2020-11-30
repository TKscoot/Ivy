#include "ivypch.h"
#include "Framebuffer.h"

Ivy::Framebuffer::Framebuffer(uint32_t width, uint32_t height)
	: mWidth(width)
	, mHeight(height)
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
