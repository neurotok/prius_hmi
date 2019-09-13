#pragma once
#include <GLFW/glfw3.h>

typedef struct ogl_shader_program {
	GLint handler;
	char label[UINT8_MAX];
}oglProgram;

typedef struct ogl_texture {
	GLuint handler;
	char label[UINT8_MAX];
}oglTexture;

typedef struct ogl_app {
	GLFWwindow* window;
	oglProgram programs[UINT8_MAX];
	char c_program[UINT8_MAX];
	uint8_t n_programs;
	oglTexture textures[UINT8_MAX];
	char c_texture[UINT8_MAX];
	uint8_t n_textures;
	GLuint framebuffers[UINT8_MAX];
	GLuint c_framebuffer;
	uint8_t n_framebuffers;

}oglApp;

oglApp oglInit(uint16_t , uint16_t , char* );
void oglLoadTexture(oglApp *, char*, char*);
void oglUseTexture(oglApp *, char *);
void oglProgLoad(oglApp *,const char *, const char *, char *);
void oglUseProg(oglApp *, char*);
GLuint oglGetProg(oglApp *, char*);
void oglProgDelete(oglApp *app, char *label);
