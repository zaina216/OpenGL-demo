#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 texCord;
in vec3 normalCoord;
in vec3 fragPos;

uniform vec3 lightPos;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform float tra;
uniform vec3 lightColour;
void main()
{
	float ambientStrength = 0.1f;

	vec3 norm = normalize(normalCoord);
	vec3 lightDir = normalize(lightPos - fragPos); // the missile determines where it is by subtracting where it isnt

	float diff = max(dot(norm, lightColour), 0.0);
	vec3 diffuse = diff * lightColour;
	
	vec3 ambient = ambientStrength * lightColour;
	vec3 result = (ambient + diffuse) * ourColor;

	FragColor = vec4(result, 1.0f) * mix(texture(ourTexture, texCord), texture(ourTexture2, vec2(-texCord[0], texCord[1])), tra) ;
}