#version 300 es
#ifdef GL_ES
precision highp float;
#endif
out vec4 FragColor;


void main()
{
    FragColor = vec4(0.0f, 0.0f, 0.0f, 0.10f);
}
