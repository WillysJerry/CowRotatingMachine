#version 420 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

struct Light {
    vec3 pos;
    vec3 color;
    float intensity;
    float attenuation;
    float ambient;

    float specularStrength;
};



in vec3 vPos;
in vec3 vNorm;

out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform Material material;
uniform Light light;
uniform vec3 viewPos; 


void main()
{
    vec3 FragPos = vec3(model * vec4(vPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * vNorm;  

    // ambient
    vec3 ambient = light.ambient * light.color;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.pos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * light.color;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = light.specularStrength * spec * vec3(1, 1, 1);

	float attenuation = 1.0 / (1.0 + light.attenuation * pow(distance(FragPos, light.pos), 2));
	vec3 linearColor = ambient + attenuation*(diffuse + specular);
    vec3 gamma = vec3(1.0/2.2, 1.0/2.2, 1.0/2.2);
	vec3 finalColor = vec3(pow(linearColor.r, gamma.r), pow(linearColor.g, gamma.g), pow(linearColor.b, gamma.b));

	vec3 result = clamp(finalColor * material.diffuse * light.intensity, 0, 1);
    Color = vec4(result, 1.0);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}