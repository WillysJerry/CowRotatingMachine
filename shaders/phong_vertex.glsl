#version 420 core
in vec3 vPos;
in vec3 vNorm;
in vec2 vUVs;

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(vPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * vNorm;  
    UV = vUVs;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}