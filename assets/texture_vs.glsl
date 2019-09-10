#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	gl_Position = vec4(aPos, 1.0);
}
