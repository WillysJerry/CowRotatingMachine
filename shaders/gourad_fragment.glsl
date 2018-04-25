#version 420 core
out vec4 FragColor;

in vec4 Color;  
in vec2 UV;

uniform sampler2D texture;

void main()
{
    FragColor = vec4(texture( texture, UV ).rgb, 1);

    //FragColor = Color;
} 