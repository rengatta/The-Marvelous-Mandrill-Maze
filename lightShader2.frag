#version 330 core
//https://learnopengl.com/Lighting/Multiple-lights
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	vec3 color;
	bool isActive;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 100
uniform int numPointLights;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec3 color;

uniform sampler2D textureID;
uniform vec3 baseColor;
uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 text);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 text);

void main()
{    
    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // == ======================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == ======================================
    // Phase 1: Directional lighting

	vec3 text = vec3(texture(textureID, TexCoords));

    vec3 result = CalcDirLight(dirLight, norm, viewDir, text);
    // Phase 2: Point lights

    for(int i = 0; i < numPointLights; i++) {
	if(pointLights[i].isActive) 
       result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, text);    
	}


    color = result;
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 text)
{
	vec3 textureColor = text * baseColor;
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Combine results
    vec3 ambient = light.ambient * textureColor;
    vec3 diffuse = light.diffuse * diff * textureColor;
    vec3 specular = light.specular * spec * textureColor;
    return (ambient + diffuse + specular );
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 text)
{
	vec3 baseTexture = text * light.color;
	vec3 lightDir = normalize(light.position - fragPos);
	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
	// Combine results
	vec3 ambient = light.ambient * baseTexture * attenuation;
	vec3 diffuse = light.diffuse * diff * baseTexture * attenuation;
	vec3 specular = light.specular * spec * baseTexture * attenuation;
	return (ambient + diffuse + specular);

}