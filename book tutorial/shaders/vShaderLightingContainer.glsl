#version 330 core
layout (location = 0) in vec3 lightCubePos;

uniform mat4 lightModel;
uniform mat4 lightView;
uniform mat4 lightProjection;

void main()
{
	gl_Position = lightProjection * lightView * lightModel * vec4(lightCubePos, 1.0f);
}