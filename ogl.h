#pragma once
#include <GLFW/glfw3.h>

#define TEXTURES (32)
#define SHADER_PROGRAMS (32)
#define FRAMEBUFFERS (32)

typedef struct ogl_shader_program {
	GLint handler;
	char label[UINT8_MAX];
}oglProgram;

typedef struct ogl_texture {
	GLuint handler;
	char label[UINT8_MAX];
}oglTexture;

typedef struct ogl_frame_buffer {
	GLuint handler;
	char label[UINT8_MAX];
}oglFramebuffer;

typedef struct ogl_app {
	GLFWwindow* window;
	uint16_t window_width, window_height;
	oglProgram programs[SHADER_PROGRAMS];
	char c_program[UINT8_MAX];
	uint8_t n_programs;
	oglTexture textures[TEXTURES];
	char c_texture[UINT8_MAX];
	uint8_t n_textures;
	oglFramebuffer framebuffers[FRAMEBUFFERS];
	char c_framebuffer[UINT8_MAX];
	uint8_t n_framebuffers;
}oglApp;

oglApp oglInit(uint16_t , uint16_t , char* );
void oglLoadTexture(oglApp *, char*, char*);
void oglUseTexture(oglApp *, char *);
GLuint oglGetTexture(oglApp *app, char *);
void oglProgLoad(oglApp *,const char *, const char *, char *);
void oglUseProg(oglApp *, char*);
GLuint oglGetProg(oglApp *, char*);
void oglProgDelete(oglApp *, char *);
void oglLoadFramebuffer(oglApp *, char *);
void oglUseFramebuffer(oglApp *app, char *label);
GLuint oglGetFramebuffer(oglApp *app, char *label);
