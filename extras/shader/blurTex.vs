#version 420 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCord;

uniform vec2 xy;
out vec2 XY;

out vec2 TexCord;

void main()
{
	XY = xy;
	TexCord = aTexCord;
	gl_Position = vec4(aPos.xy,0.0f,1.0f);
}