#include "ivypch.h"
#include "Renderer.h"

void Ivy::Renderer::Initialize()
{
	EnableDebugMessages();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// DEBUG TRIANGLE
	float vertices[] = {
		// positions          // colors				    // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f, 1.0f,   0.0f, 1.0f    // top left 
	};

	std::array<uint32_t, 6> indices = { 
	0, 1, 3,
	1, 2, 3
	};

	mShader = CreatePtr<Shader>("shaders\\Triangle.vert", "shaders\\Triangle.frag");
	//mShader->Bind();

	// Create Vertex- and Indexbuffer
	mVertexBuffer = CreatePtr<VertexBuffer>(vertices, sizeof(vertices));
	BufferLayout layout = {
		{ShaderDataType::Float3, "aPosition"},
		{ShaderDataType::Float4, "aColor"},
		{ShaderDataType::Float2, "aTexCoord"}
	};
	mVertexBuffer->SetLayout(layout);

	mIndexBuffer = CreatePtr<IndexBuffer>(indices.data(), static_cast<uint32_t>(indices.size()));

	// Set Index- and Vertexbuffer in VertexArray
	mVertexArray = CreatePtr<VertexArray>();
	mVertexArray->SetVertexBuffer(mVertexBuffer);
	mVertexArray->SetIndexBuffer(mIndexBuffer);
	mVertexArray->Unbind(); // does not need to be unbound but it's good practice to do so

	// Create test textures
	mTexture = CreatePtr<Texture2D>("assets/textures/container.jpg");
	mTexture1 = CreatePtr<Texture2D>("assets/textures/awesomeface.png");
}

void Ivy::Renderer::Render()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw our first triangle

	float timeValue = glfwGetTime();
	float colorValue = (sin(timeValue) / 2.0f) + 0.5f;

	mTexture->Bind(0);
	mTexture1->Bind(1);

	mShader->Bind(); // does not need to get bound again since theres only one shader program
	mShader->SetUniformFloat4("aColor", Vec4(Vec3(colorValue), 1.0f));

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
