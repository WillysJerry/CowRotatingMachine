#version 420 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos; 
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float lightIntensity;
uniform float lightAttenuation;
uniform float lightAmbient;

void main()
{
    // ambient
    vec3 ambient = lightAmbient * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.8f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1, 1, 1);

	float attenuation = 1.0 / (1.0 + lightAttenuation * pow(distance(FragPos, lightPos), 2));
	vec3 linearColor = ambient + attenuation*(diffuse + specular);
    vec3 gamma = vec3(1.0/2.2, 1.0/2.2, 1.0/2.2);
	vec3 finalColor = vec3(pow(linearColor.r, gamma.r), pow(linearColor.g, gamma.g), pow(linearColor.b, gamma.b));

	vec3 result = clamp(finalColor * objectColor * lightIntensity, 0, 1);
    FragColor = vec4(result, 1.0);
} 