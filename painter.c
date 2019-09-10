
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#include "painter.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

//#define SIZE           (1.0)   // determines size of window
//#define SCALE          (5)     // determines how quickly branches shrink (higher value means faster shrinking)
//#define BRANCHES       (14)    // number of branches
//#define ROTATION_SCALE (0.75)  // determines how slowly the angle between branches shrinks (higher value means slower shrinking)
//#define INITIAL_LENGTH (0.18)    // length of first branch

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
