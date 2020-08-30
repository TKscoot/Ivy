#include "ivypch.h"
#include "Renderer.h"

void Ivy::Renderer::Initialize()
{
	EnableDebugMessages();

	// DEBUG TRIANGLE
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	std::array<uint32_t, 6> indices = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3,   // second triangle
	};

	mShader = CreatePtr<Shader>("shaders\\Triangle.vert", "shaders\\Triangle.frag");
	mShader->Bind();

	// Create Vertex- and Indexbuffer
	mVertexBuffer = CreatePtr<VertexBuffer>(vertices, sizeof(vertices));
	mIndexBuffer = CreatePtr<IndexBuffer>(indices.data(), static_cast<uint32_t>(indices.size()));

	// Set Index- and Vertexbuffer in VertexArray
	mVertexArray = CreatePtr<VertexArray>();
	mVertexArray->SetVertexBuffer(mVertexBuffer);
	mVertexArray->SetIndexBuffer(mIndexBuffer);
	mVertexArray->Unbind(); // does not need to be unbound but it's good practice to do so
}

void Ivy::Renderer::Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw our first triangle

	//mShader->Bind(); // does not need to get bound again since theres only one shader program

	mVertexArray->Bind(); // only needs to get bound because it got unbound because of good practice

	glDrawElements(GL_TRIANGLES, mIndexBuffer->GetCount(), GL_UNSIGNED_INT, 0);

	// mVertexArray->Unbind(); // no need to unbind it every time 

}

void Ivy::Renderer::GLLogCallback(
	unsigned source, 
	unsigned type, 
	unsigned id, 
	unsigned severity, 
	int length, 
	const char * message, 
	const void * userParam)
{
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         
			Debug::CoreCritical(message); 
			return;
		case GL_DEBUG_SEVERITY_MEDIUM:       
			Debug::CoreError(message); 
			return;
		case GL_DEBUG_SEVERITY_LOW:          
			Debug::CoreWarning(message); 
			return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: 
			Debug::CoreLog(message); 
			return;
	}

}

void Ivy::Renderer::EnableDebugMessages()
{
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLLogCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
}
