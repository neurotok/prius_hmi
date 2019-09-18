
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#include <GLES3/gl3.h>

#include "painter.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

static inline double rand_fl(void){
	return (double)rand() / (double)RAND_MAX;
}
static void draw_branches(float offsetx, float  offsety, float  directionx, float directiony, float  size,	float  rotation, int depth, float *random_branches, uint32_t *branches_no, uint8_t scale, float rotation_scale) {
	random_branches[*branches_no] = offsetx;
	random_branches[*branches_no + 1] = offsety;
	random_branches[*branches_no + 2] = offsetx + directionx * size;
	random_branches[*branches_no + 3] = offsety + directiony * size; 
	*branches_no+=4;
	if (depth > 0){
		// draw left branch
		draw_branches(
				offsetx + directionx * size,
				offsety + directiony * size,
				directionx * cos(rotation) + directiony * sin(rotation),
				directionx * -sin(rotation) + directiony * cos(rotation),
				size * rand_fl() / scale + size * (scale - 1) / scale,
				rotation * rotation_scale,
				depth - 1, random_branches,
				branches_no,
				scale,
				rotation_scale);

		// draw right branch
		draw_branches(
				offsetx + directionx * size,
				offsety + directiony * size,
				directionx * cos(-rotation) + directiony * sin(-rotation),
				directionx * -sin(-rotation) + directiony * cos(-rotation),
				size * rand_fl() / scale + size * (scale - 1) / scale,
				rotation * rotation_scale,
				depth - 1, random_branches,
				branches_no,
				scale,
				rotation_scale);
	}
}

oglTree oglGrowTree(float size,uint8_t branches, uint8_t scale, float rotation_scale ){

	oglTree tree;	
	tree.branches_no = ((uint32_t)pow(2.0, branches + 1.0) - 1.0) * 4;
	tree.random_branches = calloc(tree.branches_no, sizeof(float));
	srand(time( NULL ));
	tree.branches_no = 0;
	draw_branches(
			0.0,
			1.0,
			0.0,
			-1.0,
			size,
			M_PI / 8,
			branches,
			tree.random_branches,
			&(tree.branches_no),
			scale,
			rotation_scale);

	return tree;

}

void oglCutTree(oglTree *tree){

	free(tree->random_branches);
}

GLuint oglLoadQuad(){

	GLuint quad_vao, quad_vbo, quad_ebo;

	glGenVertexArrays(1, &quad_vao);
	glGenBuffers(1,&quad_vbo);
	glGenBuffers(1,&quad_ebo);

	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(generic_quad), generic_quad, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(generic_quad_indices), generic_quad_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2,2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	return quad_vao;

}

GLuint oglLoadTree(oglTree *tree){

	GLuint tree_vao, tree_vbo;
	glGenVertexArrays(1, &tree_vao);
	glBindVertexArray(tree_vao);

	glGenBuffers(1, &tree_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tree_vbo);
	glBufferData(GL_ARRAY_BUFFER, (tree->branches_no * 4) * sizeof(float), tree->random_branches, GL_STATIC_DRAW);
	glVertexAttribPointer(0,2,GL_FLOAT, GL_FALSE, 2 * sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	
	return tree_vao;
}

void oglDrawTree(oglTree *tree){
	glDrawArrays(GL_LINES, 0, tree->branches_no);
}

void oglDrawQuad(void){
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

