#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GLES3/gl3.h>

#include "ogl.h"
#include "stb_image.h"
#include "stretchy_buffer.h"

oglApp oglInit(uint16_t window_width, uint16_t window_height, char* window_title){

	oglApp app; // = malloc(sizeof(oglApp));

	if (!glfwInit())
		exit(1);

	app.window_width = window_width;
	app.window_height = window_height;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	app.window =  glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);

	if (!app.window){
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(app.window);

	app.n_textures = 0;	
	app.n_programs = 0;	
	app.n_framebuffers = 0;

	app.buffers = NULL;


	return app;
}

void oglLoadTexture(oglApp *app, char*filename, char *label){

	uint8_t n = app->n_textures;

	if (n == TEXTURES) {
		printf("ERROR: Maximum number of textures %d has been reached\n",TEXTURES);	
		exit(1);
	}

	GLuint *tex = &(app->textures[n].handler);
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (filename != NULL) {
		int width, height, nrChannels;
		unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			printf("Failed to load texture\n");
		}
		stbi_image_free(data);	
	}
	else{
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, app->window_width, app->window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
		glGenerateMipmap(GL_TEXTURE_2D);

	}

	strcpy(app->textures[0].label, label);
	app->n_textures++;
}

void oglLoadFramebuffer(oglApp *app, char *label){

	uint8_t n = app->n_framebuffers;

	if (n == TEXTURES) {
		printf("ERROR: Maximum number of framebuffers %d has been reached\n",FRAMEBUFFERS);	
		exit(1);
	}

	GLuint fb;
	glGenFramebuffers (1, &fb);
	oglLoadTexture(app, NULL, "fb_tex");
	oglUseTexture(app,"fb_tex");
	glBindFramebuffer (GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, oglGetTexture(app,"fb_tex"), 0);
	GLenum status = glCheckFramebufferStatus (GL_FRAMEBUFFER);
	if (GL_FRAMEBUFFER_COMPLETE != status){
		fprintf (stderr, "ERROR: incomplete framebuffer\n");
	}
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	app->framebuffers[app->n_framebuffers].handler = fb;
	strcpy(app->framebuffers[0].label, label);
	app->n_framebuffers++;
	/*
	   printf("Framebufer %s was loaded with a GLuint %d, currently there are %d framebuffers in the app\n",
	   label,
	   app->framebuffers[app->n_framebuffers - 1].handler,
	   app->n_framebuffers);
	   */
}

void oglUseFramebuffer(oglApp *app, char *label){

	if (label != NULL) {
		if (strcmp(app->c_framebuffer,label) != 0) {
			int i;
			for (i = 0; i < FRAMEBUFFERS; ++i) {
				if (strcmp(app->framebuffers[i].label, label) == 0) {
					glBindFramebuffer (GL_FRAMEBUFFER, app->framebuffers[i].handler);
					strcpy(app->c_framebuffer, label);
					break;	
				}		
			}	
			if (i == FRAMEBUFFERS) {
				printf("ERROR: could not find \"%s\" framebuffer\n",label);
			}
		}	
	}
	else{
		glBindFramebuffer (GL_FRAMEBUFFER, 0);
		memset(app->c_framebuffer, 0, sizeof(uint8_t));

	}

}
/*
GLuint oglGetFramebuffer(oglApp *app, char *label){

	int i;
	for (i = 0; i < FRAMEBUFFERS; ++i) {
		if (strcmp(app->framebuffers[i].label, label) == 0) {
			return app->framebuffers[i].handler;
		}		
	}	
	if (i == FRAMEBUFFERS) {
		printf("ERROR: could not get \"%s\" framebuffer\n",label);
	}
	return 0;
}
*/
void oglUseTexture(oglApp *app, char *label){

	if (strcmp(app->c_texture,label) != 0) {
		int i;
		for (i = 0; i < TEXTURES; ++i) {
			if (strcmp(app->textures[i].label, label) == 0) {
				glBindTexture(GL_TEXTURE_2D, app->textures[i].handler);
				//memset(app->c_texture, 0, sizeof(uint8_t));
				strcpy(app->c_texture, label);
				break;	
			}		
		}	
		if (i == TEXTURES) {
			printf("ERROR: could not find %s texture\n",label);
		}

	}
}


static size_t fileGetLenght(FILE *file){

	size_t lenght;
	size_t currPos = ftell(file);
	fseek(file, 0 , SEEK_END);
	lenght = ftell(file);
	fseek(file, currPos, SEEK_SET);
	return lenght;
}




static GLuint oglShaderLoad(const char *filename, GLenum shaderType){

	FILE *file = fopen(filename,"r");

	if (!file) {
		printf("Can't open file: %s\n",filename);
		return 0;
	}

	size_t lenght = fileGetLenght(file);

	GLchar *shaderSrc = (GLchar *)calloc(lenght + 1, 1);
	if (!shaderSrc) {
		printf("Out of memory when reading file: %s\n",filename);
		fclose(file);
		file = NULL;
		return 0;;
	}

	fread(shaderSrc,1,lenght,file);
	fclose(file);
	file = NULL;

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const GLchar **)&shaderSrc, NULL);
	free(shaderSrc);
	shaderSrc = NULL;

	glCompileShader(shader);
	GLint compileSucceeded = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSucceeded);
	if (!compileSucceeded) {
		printf("Compilation of shader %s failed:\n",filename);
		GLint logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		GLchar *errLog = (GLchar *)malloc(logLength);
		if (errLog) {
			glGetShaderInfoLog(shader,logLength, &logLength, errLog);
			printf("%s\n",errLog);
			free(errLog);
		}
		else{
			printf("Couldn't get shader log, out of memory\n");
		}
		glDeleteShader(shader);
		shader = 0;
	}
	return shader;
}

static void oglShaderDelete(GLuint shaderID){
	glDeleteShader(shaderID);
}

void oglProgLoad(oglApp *app, const char *vertFilename, const char *fragFilename, char *label){

	uint8_t n = app->n_programs;

	if (n == SHADER_PROGRAMS) {
		printf("ERROR: Maximum number of shader programs %d has been reached\n",SHADER_PROGRAMS);	
		exit(1);
	}

	GLuint vertShader = oglShaderLoad(vertFilename, GL_VERTEX_SHADER);
	if (!vertShader) {
		printf("Couldn't load vertex shader: %s\n",vertFilename);
		exit(1);
	}
	GLuint fragShader = oglShaderLoad(fragFilename, GL_FRAGMENT_SHADER);
	if (!fragShader) {
		printf("Couldn't load fragment shader: %s\n",fragFilename);
		exit(1);
	}

	GLuint shaderProg = glCreateProgram();

	if (shaderProg) {
		glAttachShader(shaderProg,vertShader);
		glAttachShader(shaderProg,fragShader);
		glLinkProgram(shaderProg);
		GLint linkingSucceeded = GL_FALSE;
		glGetProgramiv(shaderProg, GL_LINK_STATUS, &linkingSucceeded);

		if (!linkingSucceeded) {
			printf("Linking shader failed (vet. shader: %s, frag shader: %s)\n",vertFilename, fragFilename);
			GLint logLength = 0;
			glGetProgramiv(shaderProg, GL_INFO_LOG_LENGTH, &logLength);
			GLchar *errLog = (GLchar *)malloc(logLength);
			if (errLog) {
				glGetProgramInfoLog(shaderProg, logLength, &logLength, errLog);
				printf("%s\n",errLog);
				free(errLog);
			}else{
				printf("Couldn't get shader link log, out of memory\n");
			}
			glDeleteProgram(shaderProg);
			shaderProg = 0;
		}
	}
	else{
		printf("Couldn't create shader program\n");
	}
	oglShaderDelete(vertShader);
	oglShaderDelete(fragShader);
	app->programs[app->n_programs].handler = shaderProg;
	strcpy(app->programs[app->n_programs].label, label);
	app->n_programs++;
	/*
	   printf("Shader program %s was loaded with a GLuint %d, currently there are %d programs in the app\n",
	   label,
	   app->programs[app->n_programs - 1].handler,
	   app->n_programs);
	   */
}
GLuint oglGetTexture(oglApp *app, char *label){

	int i;
	for (i = 0; i < TEXTURES; ++i) {
		if (strcmp(app->textures[i].label, label) == 0) {
			return app->textures[i].handler;
		}		
	}	
	if (i == TEXTURES) {
		printf("ERROR: could not get \"%s\" texture\n",label);
	}
	return 0;
}

GLuint oglGetProg(oglApp *app, char *label){

	int i;
	for (i = 0; i < SHADER_PROGRAMS; ++i) {
		if (strcmp(app->programs[i].label, label) == 0) {
			return app->programs[i].handler;
		}		
	}	
	if (i == SHADER_PROGRAMS) {
		printf("ERROR: could not get \"%s\" shader program\n",label);

	}
	return 0;
}

void oglUseProg(oglApp *app, char *label){

	if (strcmp(app->c_program,label) != 0) {
		int i;
		for (i = 0; i < SHADER_PROGRAMS; ++i) {
			if (strcmp(app->programs[i].label, label) == 0) {
				glUseProgram(app->programs[i].handler);
				//memset(app->c_program, 0, sizeof(uint8_t));
				strcpy(app->c_program, label);
				break;	
			}		
		}	
		if (i == SHADER_PROGRAMS) {
			printf("ERROR: could not find \"%s\" shader program\n",label);
		}

	}
}

void oglProgDelete(oglApp *app, char *label){

	int i;
	for (i = 0; i < SHADER_PROGRAMS; ++i) {
		if (strcmp(app->programs[i].label, label) == 0) {
			glDeleteProgram(app->programs[i].handler);
			break;	
		}		
	}	
	if (i == SHADER_PROGRAMS) {
		printf("ERROR: could not delete \"%s\" shader program\n",label);
	}

}

void oglUseBuffer(oglApp *app, char *label){

		if (strcmp(app->c_buffer,label) != 0) {
			int i;
			int n = stb_sb_count(app->buffers);
			for (i = 0; i < n; ++i) {
				if (strcmp(app->buffers[i].label, label) == 0) {
					glBindVertexArray(app->buffers[i].handler);
					strcpy(app->c_buffer, label);
					break;	
				}		
			}	
			if (i == n) {
				printf("ERROR: could not find \"%s\" buffer\n",label);
			}
		}	

}

GLuint oglGetBuffer(oglApp *app, char *label){

	int i;
	int n = sb_count(app->buffers);
	for (i = 0; i < n; ++i) {
		if (strcmp(app->buffers[i].label, label) == 0) {
			return app->buffers[i].handler;
		}		
	}	
	if (i == n) {
		printf("ERROR: could not get \"%s\" buffer\n",label);

	}
	return 0;
}



