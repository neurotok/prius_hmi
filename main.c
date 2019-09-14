#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#define GLFW_INCLUDE_ES3
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "stb_image.h"

#include "ogl.h"
//#include "shader.h"
#include "painter.h"


oglApp test;


typedef struct ogl_rect{
	float vert[24];
	uint8_t indecies[6];
}oglRect;

float generic_quad[] = {
//  Position      		Color             Texcoords
    -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
     1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
     1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
};


 unsigned int generic_quad_indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};

void do_frame(void *arg){

	oglApp *app = arg;
	
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDisable(GL_BLEND);
	

	glfwSwapBuffers(app->window);
	glfwPollEvents();
}

int main(void)
{
	oglApp app = oglInit(1280, 480, "Prius HMI");

	oglProgLoad(&app,"./assets/basic_vs.glsl","./assets/basic_fs.glsl", "simple_program");
	oglProgLoad(&app,"./assets/tree_vs.glsl", "./assets/tree_fs.glsl", "tree_program");
	oglProgLoad(&app,"./assets/texture_vs.glsl", "./assets/texture_fs.glsl","texture_quad_program");

	int tex_loc = glGetUniformLocation (oglGetProg(&app,"texture_quad_program"), "texture1");	

	oglTree tree  = oglGrowTree(0.18,14, 5, 0.75);

	GLuint three_vao, tree_vbo;
	glGenVertexArrays(1, &three_vao);
	glBindVertexArray(three_vao);
	
	glGenBuffers(1, &tree_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tree_vbo);
	glBufferData(GL_ARRAY_BUFFER, (tree.branches_no * 4) * sizeof(float), tree.random_branches, GL_STATIC_DRAW);

	glVertexAttribPointer(0,2,GL_FLOAT, GL_FALSE, 2 * sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

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
	
	oglLoadFramebuffer(&app, "fb");
	oglUseFramebuffer(&app, "fb");

	glClear(GL_COLOR_BUFFER_BIT);
	
	oglUseProg(&app,"tree_program");

	glBindVertexArray(three_vao);
	glDrawArrays(GL_LINES, 0, tree.branches_no);

	oglUseFramebuffer(&app,NULL);

	glBindVertexArray(three_vao);

	glClearColor(0.56f, 0.72f, 0.69f, 1.0f);
	oglUseProg(&app,"texture_quad_program");
	oglUseTexture(&app,"fb_tex");
	glUniform1i (tex_loc, 0);
	glBindVertexArray(quad_vao);
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(do_frame,&app, -1, 1);
#else
	while (!glfwWindowShouldClose(app.window))
	{
		do_frame(&app);
	}
#endif 

	oglCutTree(&tree);
	
	return 0;
}
