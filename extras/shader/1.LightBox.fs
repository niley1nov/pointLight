#version 420 core

layout(location = 1) out vec4 FragColor;
layout(location = 0) out vec4 BrightColor;

in vec3 Color;

void main()
{
	FragColor = vec4(Color,1.0f);
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	//if(brightness > 0.7f)
		BrightColor = vec4(FragColor.rgb, 1.0)*brightness;
		BrightColor *= brightness;
		BrightColor *= brightness;
}