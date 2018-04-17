#version 420 core

in float intensity;
in vec4 color;

void main()
{
	vec4 newCol;
	if (intensity > 0.95)
		newCol = vec4(color.x,1.0,1.0,1.0);
	else if (intensity > 0.5)
		newCol = vec4(color.x,0.6,0.6,1.0);
	else if (intensity > 0.25)
		newCol = vec4(color.x,0.4,0.4,1.0);
	else
		newCol = vec4(color.x,0.2,0.2,0.2);
	gl_FragColor = newCol;
}