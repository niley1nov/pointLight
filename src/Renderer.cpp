#include "Renderer.h"
#include"Shader_s.h"
#include"VertexArray.h"
#include"Texture.h"

std::string convert(int a)
{
	char x[9];
	sprintf_s(x, "%x", a);
	std::string zero = "0";
	std::string str = x;
	int len = str.size();
	for (; len < 4; len++)
	{
		str = zero + str;
	}
	return str;
}

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char * function, const char * file, int LineNo)
{
	while (GLenum error = glGetError())
	{
		std::string err = convert(error);
		std::ifstream glew("src/Error.list");
		if (glew.fail())
		{
			std::cout << "Error file didn't open" << std::endl;
			glew.clear();
		}
		std::string line;
		while (getline(glew, line))
		{
			if (line.find(err.c_str()) != std::string::npos)
			{
				std::cout << line << std::endl;
			}
		}
		std::cout << function << " " << file << " : " << LineNo << std::endl;
		return false;
	}
	return true;
}

void Renderer::Draw(GLenum mode,VertexArray& va, Shader& shader,const int& count,int first)
{
	shader.use();
	va.Bind();
	GLCall(glDrawArrays(mode, first, count));
}

void Renderer::DrawInstanced(GLenum mode, VertexArray & va, Shader & shader, const int & count, unsigned int primeCount, int first)
{
	shader.use();
	va.Bind();
	GLCall(glDrawArraysInstanced(mode, first, count, primeCount));
}


