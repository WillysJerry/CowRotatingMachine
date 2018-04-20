#version 420 core
#define NR_LIGHTS 2

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

struct Light {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float attenuation;
};

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos; 

uniform Material material;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos; 

vec3 calcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    // ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * material.diffuse * diff;
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess);
    vec3 specular = light.specular * material.specular * spec;

    float attenuation = 1.0 / (1.0 + light.attenuation * pow(distance(fragPos, light.pos), 2));

	/*
    float attenuation = 1.0 / (1.0 + light.attenuation * pow(distance(fragPos, light.pos), 2));
    vec3 linearColor = ambient + attenuation*(diffuse + specular);
    vec3 gamma = vec3(1.0/2.2, 1.0/2.2, 1.0/2.2);
	vec3 finalColor = vec3(pow(linearColor.r, gamma.r), pow(linearColor.g, gamma.g), pow(linearColor.b, gamma.b));
    */

    vec3 finalColor = attenuation * (ambient + diffuse + specular);
	return clamp(finalColor, 0, 1);

}

void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);


    vec3 result = vec3(0, 0, 0);
    for(int i = 0; i < NR_LIGHTS; i++) {
        result += calcPointLight(lights[i], norm, FragPos, viewDir);
    }
    FragColor = clamp(vec4(result, 1.0), 0, 1);
} 