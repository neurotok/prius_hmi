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

enum {
	WINDOW_WIDTH = 1280,
	WINDOW_HEIGHT = 480
};

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
	

	//glBindTexture(GL_TEXTURE_2D,0);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(app->window);
	glfwPollEvents();
}

int main(void)
{

	oglApp app = oglInit(WINDOW_WIDTH, WINDOW_HEIGHT, "Prius HMI");

	GLuint simple_program = oglProgLoad(&app,"./assets/basic_vs.glsl","./assets/basic_fs.glsl");
	if (!simple_program) return EXIT_FAILURE;

	GLuint tree_program = oglProgLoad(&app,"./assets/tree_vs.glsl", "./assets/tree_fs.glsl");
	if (!tree_program) return EXIT_FAILURE;
		
 	
	GLuint texture_quad_program = oglProgLoad(&app,"./assets/texture_vs.glsl", "./assets/texture_fs.glsl");
	if (!texture_quad_program) return EXIT_FAILURE;

	int tex_loc = glGetUniformLocation (texture_quad_program, "texture1");	

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


	GLuint grass = oglLoadTexture(&app,"./assets/grass.png");	
	
	GLuint fb;
	glGenFramebuffers (1, &fb);
	GLuint fb_tex;
	glGenTextures (1, &fb_tex); 
	glBindTexture (GL_TEXTURE_2D, fb_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindFramebuffer (GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_tex, 0);

	GLenum status = glCheckFramebufferStatus (GL_FRAMEBUFFER);
	if (GL_FRAMEBUFFER_COMPLETE != status){
		fprintf (stderr, "ERROR: incomplete framebuffer\n");
	}

	//glBindFramebuffer (GL_FRAMEBUFFER, fb);
	glClear(GL_COLOR_BUFFER_BIT);
	
	oglUseProg(&app,tree_program);	
	//glUseProgram(tree_program);
	glBindVertexArray(three_vao);
	glDrawArrays(GL_LINES, 0, tree.branches_no);

	glBindFramebuffer (GL_FRAMEBUFFER, 0);

	glBindVertexArray(three_vao);

	glClearColor(0.56f, 0.72f, 0.69f, 1.0f);
	glUseProgram(texture_quad_program);
	//oglUseTexture(&app,grass);
	//glBindTexture(GL_TEXTURE_2D, app.textures[0].handler);
	//glBindTexture(GL_TEXTURE_2D,2);
	glBindTexture(GL_TEXTURE_2D,fb_tex);
	glUniform1i (tex_loc, 0);
	glBindVertexArray(quad_vao);
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(do_frame,&app, -1, 1);
	//emscripten_set_main_loop(do_frame, 0, 1);
#else
	while (!glfwWindowShouldClose(app.window))
	{
		do_frame(&app);
	}
#endif 

	oglCutTree(&tree);
	oglProgDelete(texture_quad_program);	
	
	return 0;
}
