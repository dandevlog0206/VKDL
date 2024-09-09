#version 450 core

layout(location = 0) in vec2 Pos;
layout(location = 1) in vec4 Color;
layout(push_constant) uniform PushConstant { mat3x3 transform; } pc;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out vec4 outColor;

void main()
{
	vec3 vert   = pc.transform * vec3(Pos, 1);
	gl_Position = vec4(vert.x / vert.z, vert.y / vert.z, 0, 1);
	
	outColor = Color;
}