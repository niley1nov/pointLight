#version 420 core
layout(location = 0)in vec3 aPos;
layout(location = 1)in vec3 gar;
layout(location = 2)in vec2 bage;
layout(Location = 3)in mat4 aMats;

//layout(std140,binding = 0) uniform Matrices
//{
//	mat4 lightSpaceMat;
//};

void main()
{	
	gl_Position =  aMats * vec4(aPos,1.0f);
}