#pragma once

#include <stdint.h>
#include <GLFW/glfw3.h>

static const float generic_quad[] = {
	//  Position      		Color             Texcoords
	-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
	1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
	1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
	-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
};

static const unsigned int generic_quad_indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};

typedef struct ogl_tree{
	uint32_t branches_no; 
	float *random_branches;
}oglTree;

oglTree oglGrowTree(float size,uint8_t branches, uint8_t scale, float rotation_scale);
void oglCutTree(oglTree *tree);


GLuint oglLoadQuad(void);
void oglDrawQuad(void);

GLuint oglLoadTree(oglTree *tree);
void oglDrawTree(oglTree *tree);
