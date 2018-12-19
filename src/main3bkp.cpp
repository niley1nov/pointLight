#include"Renderer.h"
#include<iostream>
#include"Shader_s.h"
#include"VertexBuffer.h"
#include"VertexBufferLayout.h"
#include"VertexArray.h"
#include"Texture.h"
#include"Camera.h"
#include"FrameBuffer.h"
#include"UniformBuffer.h"

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float lastTime = 0.0f, deltaTime = 0.0f;
bool firstTime = true;
const float _NEAR = 0.1f;
const float _FAR = 100.0f;

void processInput(GLFWwindow*);
void framebuffer_size_callback(GLFWwindow*, int, int);
void mouse_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);
float mode(float a);

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	const void *userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int main()
{
	if (!glfwInit())
	{
		std::cout << "glfw can't be initialized" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "AlooMatar", NULL/*glfwGetPrimaryMonitor()*/, NULL);
	if (!window)
	{
		std::cout << "Window creation failed" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(1);
	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	GLCall(glEnable(GL_DEPTH_TEST));
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_MULTISAMPLE);

	{

		float vertices[] = {
			// positions          // normals           // texture coords
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f
		};

		glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  2.2f, -6.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-2.8f, -2.0f, -5.7f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  2.0f, -5.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};
		
		glm::mat4 *boxModelMats;
		boxModelMats = new glm::mat4[10];
		for (unsigned int i = 0; i < 10; ++i)
		{
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			boxModelMats[i] = model;
		}

		glm::vec3 pntLights[4][4] = {
			{	glm::vec3(0.4f, 2.0f, -0.7f),
				glm::vec3(1.0f,0.0f,0.0f),
				glm::vec3(0.2f,0.5f,1.0f),
				glm::vec3(1.0f,0.0f,0.17f)
			},
			{	glm::vec3(-2.0f, 1.0f, -4.5f),
				glm::vec3(0.0f,0.0f,1.0f),
				glm::vec3(0.2f,0.5f,1.0f),
				glm::vec3(1.0f,0.0f,0.17f)
			},
			{	glm::vec3(0.0f, 0.0f, -3.0f),
				glm::vec3(1.0f,1.0f,0.0f),
				glm::vec3(0.2f,0.5f,1.0f),
				glm::vec3(1.0f,0.0f,0.17f)
			},
			{	glm::vec3(2.0f, -2.3f, -4.0f),
				glm::vec3(0.2f,1.0f,0.6f),
				glm::vec3(0.2f,0.5f,1.0f),
				glm::vec3(1.0f,0.0f,0.2f)
			}
		};

		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

		glm::mat4* lightModelMats;
		lightModelMats = new glm::mat4[4];
		for (unsigned int i = 0; i < 4; ++i)
		{
			glm::mat4 model;
			model = glm::translate(model, pntLights[i][0]);
			model = glm::scale(model, glm::vec3(0.2f));
			lightModelMats[i] = model;
		}

		float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};

		VertexArray quadVA;
		VertexBuffer quadVB(quadVertices, sizeof(quadVertices),false);
		VertexBufferLayout quadVL;
		quadVL.push<float>(2);
		quadVL.push<float>(2);
		quadVA.AddBuffNLay(quadVB, quadVL);
		quadVB.Unbind();
		quadVA.Unbind();

		unsigned int fb, screenFB;
		unsigned int TexColo1[2];
		unsigned int RendBuf1, RendBuf2;

		GLCall(glGenFramebuffers(1, &fb));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fb));

		GLCall(glGenTextures(2, TexColo1));
		GLCall(glGenRenderbuffers(1, &RendBuf1));

		for (unsigned int i = 0; i < 2; ++i)
		{
			GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, TexColo1[i]));
			GLCall(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, GL_TRUE));
			GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, TexColo1[i], NULL));
		}
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RendBuf1));
		GLCall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH,SCR_HEIGHT));

		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RendBuf1));

		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
		GLCall(glDrawBuffers(2, attachments));

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMBUFFER - Framebuffer is not complete." << std::endl;
		GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

		unsigned int pingpongFBO[2];
		unsigned int pingpongBuffer[2];
		GLCall(glGenFramebuffers(2, pingpongFBO));
		GLCall(glGenTextures(2, pingpongBuffer));
		for (unsigned int i = 0; i < 2; i++)
		{
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]));
			GLCall(glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]));
			GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0));
			
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "ERROR::FRAMBUFFER - Framebuffer is not complete." << std::endl;
			GLCall(glBindTexture(GL_TEXTURE_2D, 0));
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		}

		unsigned int blitFB;
		unsigned int blitTex[2];
		GLCall(glGenFramebuffers(1,&blitFB));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, blitFB));
		GLCall(glGenTextures(2, blitTex));
		for (unsigned int i = 0; i < 2; ++i)
		{
			GLCall(glBindTexture(GL_TEXTURE_2D, blitTex[i]));
			GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, blitTex[i], NULL));
		}
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMBUFFER - Framebuffer is not complete." << std::endl;
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

		unsigned int blitFB2;
		unsigned int blitTex2;
		GLCall(glGenFramebuffers(1, &blitFB2));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, blitFB2));
		GLCall(glGenTextures(1, &blitTex2));
		GLCall(glBindTexture(GL_TEXTURE_2D, blitTex2));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, blitTex2, NULL));

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMBUFFER - Framebuffer is not complete." << std::endl;
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));


		VertexArray va;
		VertexBuffer vb(vertices, sizeof(vertices),false);
		VertexBufferLayout vl;
		VertexBuffer InstanceVB(boxModelMats, 10 * sizeof(glm::mat4),false);
		VertexBufferLayout Ivl;
		vl.push<float>(3);
		vl.push<float>(3);
		vl.push<float>(2);
		for (unsigned int i = 0; i < 4; ++i)
			Ivl.push<float>(4);
		va.AddBuffNLay(vb, vl);
		va.AddBuffNLay(InstanceVB, Ivl);
		vb.Unbind();
		InstanceVB.Unbind();
		va.Unbind();

		VertexArray va_l;
		VertexBuffer vb_l(vertices, sizeof(vertices),false);
		VertexBufferLayout vl_l;
		VertexBuffer modelVB_l(lightModelMats, 4 * sizeof(glm::mat4),false);
		VertexBufferLayout IVL_light;
		vl_l.push<float>(3);
		vl_l.push<float>(3);
		vl_l.push<float>(2);
		for(unsigned int i=0;i<4;++i)
			IVL_light.push<float>(4);
		va_l.AddBuffNLay(vb_l, vl_l);
		va_l.AddBuffNLay(modelVB_l, IVL_light);
		vb_l.Unbind();
		modelVB_l.Unbind();
		va_l.Unbind();

		Renderer renderer;

		DepthCube DepthCubeFB[4];

		Shader boxDepthShader;
		boxDepthShader.push(GL_VERTEX_SHADER, "extras/shader/Lit/depth.vs");
		boxDepthShader.push(GL_GEOMETRY_SHADER, "extras/shader/Lit/depth.gs");
		boxDepthShader.push(GL_FRAGMENT_SHADER, "extras/shader/Lit/depth.fs");
		boxDepthShader.buildShader();

		Shader boxShader;
		boxShader.push(GL_VERTEX_SHADER, "extras/shader/box.vs");
		boxShader.push(GL_FRAGMENT_SHADER, "extras/shader/box.fs");
		boxShader.buildShader();

		Shader blurShader;
		blurShader.push(GL_VERTEX_SHADER, "extras/shader/blurTex.vs");
		blurShader.push(GL_FRAGMENT_SHADER, "extras/shader/blurTex.fs");
		blurShader.buildShader();

		blurShader.use();
		blurShader.setInt("Texture1", 0);

		boxShader.use();

		Texture t1("extras/textures/c2.jpg",GL_REPEAT);
		boxShader.setInt("material.diffuse", 0);
		boxShader.setInt("material.specular", 1);
		boxShader.setFloat("material.shininess", 128.0f);

		for(unsigned int i=0;i<4;++i)
			boxShader.pntStruct_set(pntLights[i], i);

		Shader lightShader;
		lightShader.push(GL_VERTEX_SHADER, "extras/shader/1.LightBox.vs");
		lightShader.push(GL_FRAGMENT_SHADER, "extras/shader/1.LightBox.fs");
		lightShader.buildShader();

		Shader quadShader;
		quadShader.push(GL_VERTEX_SHADER, "extras/shader/quadShader.vs");
		quadShader.push(GL_FRAGMENT_SHADER, "extras/shader/quadShader.fs");
		quadShader.buildShader();

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, _NEAR, _FAR);

		UniformBuffer uboShad;
		uboShad.push(6, sizeof(glm::mat4));
		uboShad.BuildBuffer(0, GL_DYNAMIC_DRAW);

		UniformBuffer uboOrg;
		uboOrg.push(1, sizeof(glm::mat4));
		uboOrg.push(1, sizeof(glm::vec2));
		uboOrg.BuildBuffer(1, GL_DYNAMIC_DRAW);

		lightShader.use();
		for (unsigned int i = 0; i < 4; ++i)
			lightShader.setVec3("aColor[" + std::to_string(i) + "]", pntLights[i][1]);

		glm::mat4 LitProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / SHADOW_HEIGHT, _NEAR, 15.0f);

		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = float(glfwGetTime());
			deltaTime = currentFrame - lastTime;
			lastTime = currentFrame;

			processInput(window);
			float theta = glfwGetTime();
			float a = mode(cos(theta));
			float b = mode(sin(theta));

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glCullFace(GL_BACK);

			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			boxDepthShader.use();
			// view/projection transformations
			for (unsigned int i = 0; i < 4; ++i)
			{
				std::vector<glm::mat4> shadowTrans;
				shadowTrans.push_back(LitProj*glm::lookAt(pntLights[i][0], pntLights[i][0] + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
				shadowTrans.push_back(LitProj*glm::lookAt(pntLights[i][0], pntLights[i][0] + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
				shadowTrans.push_back(LitProj*glm::lookAt(pntLights[i][0], pntLights[i][0] + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
				shadowTrans.push_back(LitProj*glm::lookAt(pntLights[i][0], pntLights[i][0] + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
				shadowTrans.push_back(LitProj*glm::lookAt(pntLights[i][0], pntLights[i][0] + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
				shadowTrans.push_back(LitProj*glm::lookAt(pntLights[i][0], pntLights[i][0] + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
				
				DepthCubeFB[i].Bind();

				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
				uboShad.Bind();
				uboShad.fillData(0, glm::value_ptr(shadowTrans[0]));
				uboShad.fillData(1, glm::value_ptr(shadowTrans[1]));
				uboShad.fillData(2, glm::value_ptr(shadowTrans[2]));
				uboShad.fillData(3, glm::value_ptr(shadowTrans[3]));
				uboShad.fillData(4, glm::value_ptr(shadowTrans[4]));
				uboShad.fillData(5, glm::value_ptr(shadowTrans[5]));
				uboShad.Unbind();

				boxDepthShader.setVec3("lightPos", pntLights[i][0]);

				glCullFace(GL_FRONT);
				renderer.DrawInstanced(GL_TRIANGLES, va, boxDepthShader, 36, 10);
				glCullFace(GL_BACK);
			}
			glBindFramebuffer(GL_FRAMEBUFFER, fb);
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			glm::mat4 view = camera.GetViewMatrix();

			boxShader.use();

			uboOrg.Bind();
			uboOrg.fillData(0, glm::value_ptr(projection * view));
			uboOrg.fillData(1, glm::value_ptr(glm::vec2(a, b)));
			uboOrg.Unbind();

			boxShader.setVec3("viewPos", camera.GetPosition()); 

			t1.Bind(0,GL_TEXTURE_2D);
			for (unsigned int i = 1; i < 5; ++i)
			{
				DepthCubeFB[i-1].BindTex(i);
				boxShader.setInt("shadMap["+std::to_string(i-1)+"]", i);
			}
			renderer.DrawInstanced(GL_TRIANGLES, va, boxShader, 36, 10);
	
			lightShader.use();
			renderer.DrawInstanced(GL_TRIANGLES, va_l, lightShader, 36, 4);

			GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, fb));
			GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blitFB));

			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			GLCall(glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR));

			glReadBuffer(GL_COLOR_ATTACHMENT1);
			glDrawBuffer(GL_COLOR_ATTACHMENT1);
			GLCall(glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR));
			
			GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, blitFB));
			GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blitFB2));
			GLCall(glReadBuffer(GL_COLOR_ATTACHMENT0));
			GLCall(glDrawBuffer(GL_COLOR_ATTACHMENT0));
			GLCall(glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH/8, SCR_HEIGHT/8, GL_COLOR_BUFFER_BIT, GL_LINEAR));

			GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, blitFB2));
			GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blitFB));
			GLCall(glReadBuffer(GL_COLOR_ATTACHMENT0));
			GLCall(glDrawBuffer(GL_COLOR_ATTACHMENT0));
			GLCall(glBlitFramebuffer(0, 0, SCR_WIDTH/8, SCR_HEIGHT/8, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR));

			bool horizontal = true, first_iteration = true;
			int amount = 10;
			for (unsigned int i = 0; i < amount; ++i)
			{
				GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]));
				blurShader.use();
				blurShader.setVec2("xy",glm::vec2(horizontal,!horizontal));
				GLCall(glActiveTexture(GL_TEXTURE0));
				GLCall(glBindTexture(GL_TEXTURE_2D, first_iteration ? blitTex[0] : pingpongBuffer[!horizontal]));
				renderer.Draw(GL_TRIANGLES,quadVA ,blurShader, 6);
				horizontal = !horizontal;
				if (first_iteration)
					first_iteration = false;
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			}

			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, NULL));
			glDisable(GL_DEPTH_TEST);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			quadShader.use();
			GLCall(glActiveTexture(GL_TEXTURE0));
			GLCall(glBindTexture(GL_TEXTURE_2D, blitTex[1]));
			GLCall(glActiveTexture(GL_TEXTURE1));
			GLCall(glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]));
			quadShader.setInt("scene", 0);
			quadShader.setInt("bloomBlur", 1);
			quadShader.setFloat("exposure", 1.0f);
			renderer.Draw(GL_TRIANGLES, quadVA, quadShader, 6);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::WUP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(camera_movement::WDOWN, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	GLCall(glViewport(0, 0, width, height));
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstTime)
	{
		lastX = xPos;
		lastY = yPos;
		firstTime = false;
	}
	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;
	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow*, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset,false);
}

float mode(float a)
{
	if (a > 0.0f)
		return a;
	return -1 * a;
}
