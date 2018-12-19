#version 420 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCord;

//uniform float m_flag;
out vec2 TexCord;
//out float flag;

void main()
{
	TexCord = aTexCord;
	gl_Position = vec4(aPos.xy,0.0f,1.0f);
	//flag = m_flag;
}