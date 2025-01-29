#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 texCord;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform float tra;


void main()
{
	FragColor = mix(texture(ourTexture, texCord), texture(ourTexture2, vec2(-texCord[0], texCord[1])), tra) * vec4(ourColor, 1.0f);
}