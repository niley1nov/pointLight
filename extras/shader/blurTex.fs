#version 420 core

layout(location = 0) out vec4 FragColor;
uniform sampler2D Texture1;
in vec2 TexCord;
in vec2 XY;

//vec2 offset = 2.0f / textureSize(Texture1,0);
float offset;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	if(XY.x>0.5f)
		offset = 8.0f / 1920.0f;
	else
		offset = 8.0f / 1080.0f;
	vec3 col = texture(Texture1,TexCord).rgb * weight[0];
	int index = 0;
	for(int i=1;i<5;++i)
	{
		col += texture(Texture1, TexCord.st + offset*vec2(i)*XY).rgb * weight[i];
		col += texture(Texture1, TexCord.st - offset*vec2(i)*XY).rgb * weight[i];
	}
	FragColor = vec4(col, 1.0);
	//FragColor = vec4(1.0f);
}