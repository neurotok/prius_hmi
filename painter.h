#pragma once

#include <stdint.h>

typedef struct ogl_tree{
	uint32_t branches_no; 
	float *random_branches;
}oglTree;

oglTree oglGrowTree(float size,uint8_t branches, uint8_t scale, float rotation_scale);
void oglCutTree(oglTree *tree);
