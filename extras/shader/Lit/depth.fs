#version 420 core

in vec4 FragPos;
uniform vec3 lightPos;
float near = 0.1f;
float far = 15.0f;

//float LinearizeDepth(float depth)
//{
//	float z = depth *2.0f -1.0f;
//	return (2.0f*near*far)/(far+near-z*(far-near))/far;
//}

//void main()
//{
//	//float depth = LinearizeDepth(gl_FragCoord.z);
//	float depth = length(FragPos.xyz-lightPos)/far;
//	float dx = dFdx(depth);
//	float dy = dFdy(depth);
//	float moment2 = depth*depth+ 0.25*(dx*dx + dy*dy);
//
//	gl_FragColor = vec4(depth,moment2,0.0f,0.0f);
//}

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos)/far;
   
    gl_FragDepth = lightDistance;
}