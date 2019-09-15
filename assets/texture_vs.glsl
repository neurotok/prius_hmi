#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

/*
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
*/
uniform mat4 transform;

out vec2 TexCoord;

void main()
{
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	//gl_Position = projection * view * model * vec4(aPos, 1.0);
	gl_Position = transform * vec4(aPos, 1.0);
}
