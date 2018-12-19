#version 420 core

out vec4 FragColor;
uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform float exposure;
in vec2 TexCord;
//in float flag;

void main()
{
	vec3 DiffColo = texture(scene,TexCord).rgb;
	vec3 bloomColo = texture(bloomBlur,TexCord).rgb;
	DiffColo +=(bloomColo*0.3f);

	//vec3 hdrColo = DiffColo/(DiffColo+vec3(1.0f));
	vec3 hdrColo = vec3(1.0f) - exp(-DiffColo*exposure);
	float gamma  = 1/2.4f;
	float a = 0.055f;
	float lim = 0.0031308;
	if(hdrColo.r<=lim)
		hdrColo.r *= 12.92;
	else
		hdrColo.r = pow((hdrColo.r),gamma)*(1+a)-a;
	if(hdrColo.g<=lim)
		hdrColo.g *= 12.92;
	else
		hdrColo.g = pow((hdrColo.g),gamma)*(1+a)-a;
	if(hdrColo.b<=lim)
		hdrColo.b *= 12.92;
	else
		hdrColo.b = pow((hdrColo.b),gamma)*(1+a)-a;
	//float gamma = 1/2.2f;
	//hdrColo = pow(hdrColo,vec3(gamma));
	FragColor = vec4(hdrColo,1.0f);
}