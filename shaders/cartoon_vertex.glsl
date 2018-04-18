#version 420 core

in vec3 vPos;
in vec3 vNorm;
out float intensity;
out vec4 color;
uniform mat4 PV;


void main()
{
	intensity = dot(vPos, vNorm);
	color = normalize(abs(vec4(vNorm, 1.0)) * PV);
	gl_Position = PV * vec4(vPos, 1.0f);
}