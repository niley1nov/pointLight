#version 420 core

layout(location = 1) out vec4 FragColor;
layout(location = 0) out vec4 BrightColor;

in VS_OUT
{
	vec3 norm;
	vec3 fragPos;
	vec2 TexCord;
}fs_in;

struct Material
{
	sampler2D diffuse;
	//sampler2D specular;
	float shininess;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PntLight
{
	vec3 position;

	vec3 Color;
	float ambient;
	float diffuse;
	float specular;

	float constant;
	float linear;
	float quadratic;
};

struct FlsLight
{
	vec3 position;
	vec3 direction;

	float inCutoff;
	float outCutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform vec3 viewPos;
uniform Material material;

#define pnt_lit_count 4
uniform DirLight dirLit;
uniform PntLight pntLit[pnt_lit_count];
uniform FlsLight flsLit;
uniform samplerCube shadMap[4];

vec4 directional(DirLight );
vec4 point(PntLight ,int);
vec4 flash(FlsLight );

vec3 colo;
in vec2 de;

void main()
{
	vec4 outColor;
	//outColor += directional(dirLit);
	for(int i=0;i<pnt_lit_count;++i)
	{	
		colo = de.x * (1.0f-pntLit[i].Color) + de.y * pntLit[i].Color;
		outColor += point(pntLit[i],i);
	}
	//outColor += flash(flsLit);
	FragColor = outColor;

	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	//if(brightness > 0.7f)
		BrightColor = vec4(FragColor.rgb, 1.0)*brightness;
		BrightColor *= brightness;
		BrightColor *= brightness;
}

float linstep(float low,float high,float v)
{
	return clamp((v-low)/(high-low),0.0,1.0);
}

float near = 0.1f;
float far = 15.0f;

float LinearizeDepth(float depth)
{
	float z = depth *2.0f -1.0f;
	return (2.0f*near*far)/(far+near-z*(far-near))/far;
}

float shadowCalculation(int i,vec3 fragPos)
{
	vec3 LitToFrag = fragPos-pntLit[i].position;
	float closestDepth = (texture(shadMap[i], LitToFrag).r)*far;
	float currentDepth = length(LitToFrag);
	//FragColor = vec4(vec3(closestDepth / far), 1.0); 
	return currentDepth > closestDepth ? 0.0f : 1.0f;
}

vec4 directional(DirLight light)
{
	//ambient
	vec3 DiffColo = texture(material.diffuse,fs_in.TexCord).rgb;
	float gamma  = 2.4f;
	float a = 0.055f;
	float lim = 0.04045;
	if(DiffColo.r<=lim)
		DiffColo.r /= 12.92;
	else
		DiffColo.r = pow((DiffColo.r + a)/(1+a),gamma);
	if(DiffColo.g<=lim)
		DiffColo.g /= 12.92;
	else
		DiffColo.g = pow((DiffColo.g + a)/(1+a),gamma);
	if(DiffColo.b<=lim)
		DiffColo.b /= 12.92;
	else
		DiffColo.b = pow((DiffColo.b + a)/(1+a),gamma);

	vec3 ambient = DiffColo * light.ambient;
	//diffuse
	vec3 lightDir = normalize(vec3(-light.direction));
	float diff = max(dot(fs_in.norm,lightDir),0.0f);
	vec3 diffusion = diff * light.diffuse * DiffColo;
	//specular
	vec3 viewDir = normalize(viewPos-fs_in.fragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(fs_in.norm,halfWayDir),0.0f),material.shininess);
	vec3 specular = light.specular * spec;
	
	return(vec4(ambient+diffusion+specular,1.0f));
}

vec4 point(PntLight light,int i)
{
	//ambient
	vec3 DiffColo = texture(material.diffuse,fs_in.TexCord).rgb;
	float gamma  = 2.4f;
	float a = 0.055f;
	float lim = 0.04045;
	if(DiffColo.r<=lim)
		DiffColo.r /= 12.92;
	else
		DiffColo.r = pow((DiffColo.r + a)/(1+a),gamma);
	if(DiffColo.g<=lim)
		DiffColo.g /= 12.92;
	else
		DiffColo.g = pow((DiffColo.g + a)/(1+a),gamma);
	if(DiffColo.b<=lim)
		DiffColo.b /= 12.92;
	else
		DiffColo.b = pow((DiffColo.b + a)/(1+a),gamma);

	//float gamma = 2.2f;
	//DiffColo = pow(DiffColo,vec3(gamma));

	vec3 ambient = light.ambient*colo*DiffColo;
	//diffuse
	vec3 lightDir = normalize(light.position - fs_in.fragPos);
	float diff = max(dot(fs_in.norm,lightDir),0.0f);
	vec3 diffusion = diff * light.diffuse * colo * DiffColo;

	//specular
	vec3 viewDir = normalize(viewPos-fs_in.fragPos);
	vec3 halfWayDir = normalize(lightDir+viewDir);
	float spec = pow(max(dot(fs_in.norm,halfWayDir),0.0f),material.shininess);
	//vec3 specular = light.specular * colo * spec * texture(material.specular,fs_in.TexCord).rgb;
	vec3 specular = light.specular * colo * spec ;
	//attenution
	float distance = length(light.position - fs_in.fragPos);
	//float attenution = 1.0f / (light.constant + light.linear*distance + light.quadratic*(distance * distance));
	float attenution = 1.0f / pow(light.constant +light.linear*distance+ light.quadratic*(distance * distance),1.0f);
	float shadow = 0.0f;
	shadow = shadowCalculation(i,fs_in.fragPos);

	return(vec4((ambient + (diffusion + specular)*shadow) *attenution , 1.0f));
	//return(vec4((ambient + (diffusion + specular)*shadow) * attenution , 1.0f));
}

vec4 flash(FlsLight light)
{
	//ambient
	vec3 ambient = texture(material.diffuse,fs_in.TexCord).rgb * light.ambient;
	//diffuse
	vec3 lightDir = normalize(light.position-fs_in.fragPos);
	float diff = max(dot(fs_in.norm,lightDir),0.0f);
	vec3 diffusion = diff * light.diffuse * vec3(texture(material.diffuse,fs_in.TexCord));
	//specular
	vec3 viewDir = normalize(viewPos-fs_in.fragPos);
	vec3 halfWayDir = normalize(lightDir+viewDir);
	float spec = pow(max(dot(fs_in.norm,halfWayDir),0.0f),material.shininess);
	vec3 specular = light.specular * spec;// * vec3(texture(material.specular,fs_in.TexCord));
	//attenution
	float distance = length(light.position - fs_in.fragPos);
	float attenution = 1.0f / (light.constant + light.linear*distance + light.quadratic*(distance * distance));
	//soft edge spotlight
	float theta = dot(lightDir,normalize(-light.direction));
	float epsilon = light.inCutoff - light.outCutoff;
	float intensity = clamp((theta - light.outCutoff) / epsilon, 0.0f,1.0f);
	diffusion *= intensity;
	specular *= intensity;

	return(vec4((ambient + diffusion + specular) * attenution , 1.0f));
}