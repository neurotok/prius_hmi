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
#include "stretchy_buffer.h"

#include "painter.h"
#include "ogl.h"


void do_frame(void *arg){

	oglApp *app = arg;

	mat4x4 transform;
	mat4x4_identity(transform);
	mat4x4_translate(transform, 0.5f, 0.0f, 0.0f);	

	glClearColor(0.56f, 0.72f, 0.69f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	oglUseProg(app,"texture_quad_program");
	oglUseTexture(app,"fb_tex");
	glUniformMatrix4fv(app->uniforms[0], 1, GL_FALSE, transform[0]);
	oglUseBuffer(app, "tree_canvas");
	oglDrawQuad();

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

	GLuint transformations[1];
	transformations[0] = glGetUniformLocation(oglGetProg(&app, "texture_quad_program"), "transform");
		
	app.uniforms = transformations;

	oglTree tree  = oglGrowTree(0.18,14, 5, 0.75);

	oglBuffer eco_tree = {
		.handler = oglLoadTree(&tree),
		.label = "eco_tree"
	};
	stb_sb_push(app.buffers,eco_tree);

	oglBuffer tree_canvas = {
		.handler = oglLoadQuad(),
		.label = "tree_canvas"
	};
	stb_sb_push(app.buffers,tree_canvas);

	oglLoadFramebuffer(&app, "fb");

	oglUseFramebuffer(&app, "fb");
	glClear(GL_COLOR_BUFFER_BIT);
	oglUseProg(&app,"tree_program");
	oglUseBuffer(&app, "eco_tree");
	oglDrawTree(&tree);	

	oglUseFramebuffer(&app,NULL);

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
