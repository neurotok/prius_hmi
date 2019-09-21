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

	uint8_t fuel_level = 100;

	mat4x4 transform;
	mat4x4_identity(transform);
	//mat4x4_translate(transform, 1.5f, 0.0f, 0.0f);	
	mat4x4_translate(transform, 2.0f -  (float)fuel_level / 100.0f * 2.0f, 0.0f, 0.0f);	

	glClearColor(0.56f, 0.72f, 0.69f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//Fuel gage
	oglUseProg(app,"simple_program");
	glUniformMatrix4fv(app->uniforms[0], 1, GL_FALSE, transform[0]);
	oglUseBuffer(app,"fuel");
	oglDrawQuad();

	//Tree canvas
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	oglUseProg(app,"texture_quad_program");
	oglUseTexture(app,"fb_tex");
	oglUseBuffer(app, "tree_canvas");

	mat4x4_identity(transform);

	glUniformMatrix4fv(app->uniforms[1], 1, GL_FALSE, transform[0]);
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

	GLuint transformations[2];
	transformations[0] = glGetUniformLocation(oglGetProg(&app, "simple_program"), "transform");
	transformations[1] = glGetUniformLocation(oglGetProg(&app, "texture_quad_program"), "transform");

	printf("%d %d\n",transformations[0], transformations[1]);


		app.uniforms = transformations;

	oglBuffer fuel = {
		//.handler = oglLoadQuad(&app, 100, 100, 800, 200, 4, 0x00FF00, 0x0000FF, 0x00FFFF, 0xFF0000),
		//.handler = oglLoadQuad(&app, 100, 100, 800, 200, 2, 0x00FF00, 0x0000FF),
		.handler = oglLoadQuad(&app, 100, 100, 800, 200, 1, 0xFF0000),
		.label = "fuel"
	}; stb_sb_push(app.buffers,fuel);

	oglTree tree = oglGrowTree(0.18,14, 5, 0.75);

	oglBuffer eco_tree = {
		.handler = oglLoadTree(&tree),
		.label = "eco_tree"
	}; stb_sb_push(app.buffers,eco_tree);

	oglBuffer tree_canvas = {
		//.handler = oglLoadQuad(&app, 100, 100, 400, 200),
		.handler = oglLoadQuad(&app, 100, 100, 300, 300, 1, 0xFFFFFF),
		.label = "tree_canvas"
	}; stb_sb_push(app.buffers,tree_canvas);

	oglLoadFramebuffer(&app, "fb");

	//Pre farame 
	oglUseFramebuffer(&app, "fb");
	glClear(GL_COLOR_BUFFER_BIT);
	oglUseProg(&app,"tree_program");
	oglUseBuffer(&app, "eco_tree");
	oglDrawTree(&tree);	

	//Back to default framebuffer
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
