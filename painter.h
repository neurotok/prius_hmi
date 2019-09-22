#pragma once

#include <stdint.h>
#include <GLFW/glfw3.h>

#include "ogl.h"

static const unsigned int generic_quad_indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};

typedef struct ogl_color{
	float r;
	float g;
	float b;
}oglColor;

typedef struct ogl_tree{
	uint32_t branches_no; 
	float *random_branches;
}oglTree;

oglTree oglGrowTree(float size,uint8_t branches, uint8_t scale, float rotation_scale);
void oglCutTree(oglTree *tree);

GLuint oglLoadQuad(oglApp *app, int x, int y, int w, int h, int nc, ...);
void oglDrawQuad(void);

GLuint oglLoadTriangle(oglApp *app, int x, int y, int w, int h, int nc, ...);
void oglDrawTriangle(void);

GLuint oglLoadTree(oglTree *tree);
void oglDrawTree(oglTree *tree);
