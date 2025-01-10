#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 atexCord;
uniform float xOffset;
out vec3 ourColor;
out vec2 texCord;
void main()
{
   gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);
   ourColor = aColor;
   texCord = vec2(atexCord.x. atexCord.y);
}