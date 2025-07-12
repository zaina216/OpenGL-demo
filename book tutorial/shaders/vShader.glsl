#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 atexCord;
layout (location = 3) in vec3 aNormalCoord;

uniform float xOffset;
uniform float transparency;
// uniform vec3 changingColour;
uniform mat4 transform;
out vec3 ourColor;
out vec2 texCord;
out vec3 normalCoord;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * transform * vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0) ;
	//   ourColor = changingColour;
	
	ourColor = aColor;

	fragPos = vec3(model * vec4(aPos, 1.0));
	texCord = atexCord;
	normalCoord = aNormalCoord;

}