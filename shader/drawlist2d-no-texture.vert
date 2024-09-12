#version 450 core

layout(push_constant) uniform PushConstant { 
	mat3x3 transform;
	vec4 color;
	vec2 vertex[4];
	vec2 uv[4];
} pc;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

const int indicies[] = { 0, 1, 2, 0, 2, 3 };

void main()
{
	int index = indicies[gl_VertexIndex];

	vec3 vert   = pc.transform * vec3(pc.vertex[index], 1);
	gl_Position = vec4(vert.x / vert.z, vert.y / vert.z, 0, 1);

	Out.Color = pc.color;
	Out.UV    = pc.uv[index];
}