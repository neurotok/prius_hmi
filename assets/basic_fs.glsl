#version 300 es
#ifdef GL_ES
precision highp float;
#endif
in vec3 ourColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
