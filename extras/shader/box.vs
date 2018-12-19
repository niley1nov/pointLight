#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCord;
layout(location = 3) in mat4 aModelMats;

layout(std140,binding = 1) uniform Matrices
{
	mat4 projView;
	//mat4 model;
	vec2 ab;
};

out VS_OUT
{
	vec3 norm;
	vec3 fragPos;
	vec2 TexCord;
}vs_out;

out vec2 de;

void main()
{
	gl_Position = projView  * aModelMats * vec4(aPos,1.0f);
	vs_out.fragPos = vec3(aModelMats * vec4(aPos,1.0f));
	vs_out.norm = normalize(mat3(transpose(inverse(aModelMats))) * aNormal);
	vs_out.TexCord = aTexCord;
	//gl_PointSize = gl_Position.z;
	de = ab;
}