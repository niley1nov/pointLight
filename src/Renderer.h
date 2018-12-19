#pragma once
#ifndef RENDER_H
#define RENDER_H

#include "GLEW/glew.h"
#include"GLFW\glfw3.h"
#include"glm\glm.hpp"
#include"glm\gtc\matrix_transform.hpp"
#include"glm\gtc\type_ptr.hpp"
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
class VertexArray;
//class IndexBuffer;
class Shader;
class Texture;

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError(); \
	x;\
	ASSERT(GLLogCall(#x,__FILE__,__LINE__))

std::string convert(int a);
void GLClearError();
bool GLLogCall(const char* function, const char* file, int LineNo);


class Renderer
{
public:
	void Draw(GLenum mode,VertexArray& va, Shader& shader,const int& count,int first = 0);
	void DrawInstanced(GLenum mode, VertexArray& va, Shader& shader, const int& count, unsigned int primeCount, int first = 0);
};

#endif