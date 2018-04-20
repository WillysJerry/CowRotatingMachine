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

float f_cartoonize(float x) {
	if(x > 0.8)
		return 1.0;
	else if(x > 0.75)
		return 0.6;
    else if(x > 0.2)
        return 0.4;
    else if(x > 0.1)
        return 0.2;
	else if(x > 0.05)
        return 0.1;

	return 0;

}

vec3 v_cartoonize(vec3 color, int n) { // 0.7, 2


    vec3 x = color * 10;        // 0.7 -> 7
    vec3 m = mod(x, n);         // 7 % 2 = 1
    vec3 y = x + m;             // 7 - 1 = 6

    return clamp(floor(y) / 10, 0, 1); // 6 / 10 = 0.6

}

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
    FragColor = vec4( f_cartoonize(result.x), f_cartoonize(result.y), f_cartoonize(result.z), 1.0);
} 