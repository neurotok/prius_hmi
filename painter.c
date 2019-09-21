
#include <stdio.h>
#include <stdarg.h>
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

GLuint oglLoadQuad(oglApp *app, int x, int y, int w, int h, int nc, ...){

	va_list args;

	int c[nc];

	va_start( args, nc );

	for( int i = 0; i < nc; i++ )
		c[i] = va_arg( args, int );
	va_end( args );

	oglColor col[nc]; 

	for (int i = 0; i < nc; ++i) {
		
		col[i].r = ((c[i] >> 16) & 0xFF) / 255.0;
		col[i].g = ((c[i] >> 8) & 0xFF) / 255.0;
		col[i].b = ((c[i]) & 0xFF) / 255.0;

	}


	float wc = 2.0f * (float)w / (float)(app->window_width);
	float hc = 2.0f * (float)h / (float)(app->window_height);
	float xc = 2.0f * (float)x / (float)(app->window_width);
	float yc = 2.0f * (float)y / (float)(app->window_height);

	float quad[] = {
		-1.0f + xc, 1.0f - yc, 0.0f, col[0].r, col[0].g, col[0].b, 0.0f, 0.0f,
		-1.0f + xc + wc, 1.0f - yc, 0.0f, col[0].r, col[0].g, col[0].b, 1.0f, 0.0f,
		-1.0f + xc + wc, 1.0f - yc - hc, 0.0f, col[0].r, col[0].g, col[0].b, 1.0f, 1.0f,
		-1.0f + xc, 1.0f - yc - hc, 0.0f, col[0].r, col[0].g, col[0].b, 0.0f, 1.0f
	};


	switch (nc) {
		case 2:
			quad[19] = col[1].r; quad[20] = col[1].g; quad[21] = col[1].b; //bottom right
			quad[27] = col[1].r; quad[28] = col[1].g; quad[29] = col[1].b; //bottom left
		break;
		case 4:
			quad[11] = col[1].r; quad[12] = col[1].g; quad[13] = col[1].b; //top right
			quad[19] = col[3].r; quad[20] = col[3].g; quad[21] = col[3].b; //bottom right	
			quad[27] = col[2].r; quad[28] = col[2].g; quad[29] = col[2].b; //bottom left	
		break;
		default:
			break;
			
	}
	
	GLuint quad_vao, quad_vbo, quad_ebo;

	glGenVertexArrays(1, &quad_vao);
	glGenBuffers(1,&quad_vbo);
	glGenBuffers(1,&quad_ebo);

	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

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
	glLineWidth(2);
	glDrawArrays(GL_LINES, 0, tree->branches_no);
}

void oglDrawQuad(void){
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


