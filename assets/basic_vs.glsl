#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 transform;

void main()
{
	ourColor = aColor;
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
