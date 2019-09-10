#pragma once
#include <GLFW/glfw3.h>

typedef struct ogl_shader {
	GLint shader_program;
	uint8_t id;
}oglShader;

typedef struct ogl_app {
	GLFWwindow* window;
	GLuint shader_programs[UINT8_MAX];
	GLuint c_program;
	uint8_t n_programs;
	GLuint textures[UINT8_MAX];
	GLuint c_texture;
	uint8_t n_textures;
	GLuint framebuffers[UINT8_MAX];
	GLuint c_framebuffer;
	uint8_t n_framebuffers;

}oglApp;

oglApp oglInit(uint16_t , uint16_t , char* );
GLuint oglLoadTexture(oglApp *, char*);
void oglUseTexture(oglApp *, GLuint);
GLuint oglProgLoad(oglApp *,const char *, const char *);
void oglProgDelete(GLuint);
void oglUseProg(oglApp *, GLuint );
