#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 gar;
layout(location = 2) in vec2 bage;
layout(location = 3) in mat4 aModel;

layout(std140,binding = 1) uniform Matrices
{
	mat4 projView;
	//mat4 model;
	vec2 ab;
};

uniform vec3 aColor[4];
out vec3 Color;

void main()
{   Color = ab.x * (1.0f-aColor[gl_InstanceID]) + ab.y * aColor[gl_InstanceID];
	gl_Position = projView  * aModel * vec4(aPos,1.0f);
	//gl_PointSize = gl_Position.z;
}