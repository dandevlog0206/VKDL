#version 450 core

layout(location = 0) in vec4 Color;
layout(location = 0) out vec4 fColor;

void main()
{
	fColor = Color;
}