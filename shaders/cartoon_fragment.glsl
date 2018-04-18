#version 420 core

in float intensity;
in vec4 color;

float limit(float x) {
	if(x > 0.8)
		return 1.0;
	else if(x > 0.75)
		return 0.6;
	else if(x > 0)
		return 0;

	return 0;
}

void main()
{
	vec4 newCol = vec4(limit(color.r), limit(color.g), limit(color.b), 1.0);
	gl_FragColor = newCol;
}