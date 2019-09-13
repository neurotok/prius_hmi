#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GLES3/gl3.h>

#include "ogl.h"
#include "stb_image.h"

oglApp oglInit(uint16_t window_width, uint16_t window_height, char* window_title){

	oglApp app; // = malloc(sizeof(oglApp));

	if (!glfwInit())
		exit(1);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	app.window =  glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);

	if (!app.window){
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(app.window);
	memset(app.textures, 0,sizeof(oglTexture) * UINT8_MAX);
	app.n_textures = 0;	
	memset(app.c_texture, 0, sizeof(uint8_t));

	memset(app.programs, 0,sizeof(oglProgram) * UINT8_MAX);
	app.n_programs = 0;	
	memset(app.c_program, 0, sizeof(uint8_t));

	return app;
}
void oglLoadTexture(oglApp *app, char*filename, char *label){

	uint8_t n = app->n_textures;

	if (n == UINT8_MAX) {
		printf("ERROR: Maximum number of textures %d has been reached\n",UINT8_MAX);	
		exit(1);
	}

	GLuint *tex = &(app->textures[n].handler);
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
	strcpy(app->textures[0].label, label);
	app->n_textures++;
}

void oglUseTexture(oglApp *app, char *label){

	if (strcmp(app->c_texture,label) != 0) {
		int i;
		for (i = 0; i < UINT8_MAX; ++i) {
			if (strcmp(app->textures[i].label, label) == 0) {
				glBindTexture(GL_TEXTURE_2D, app->textures[i].handler);
				memset(app->c_texture, 0, sizeof(uint8_t));
				strcpy(app->c_texture, label);
				break;	
			}		
		}	
		if (i == UINT8_MAX) {
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

	if (n == UINT8_MAX) {
		printf("ERROR: Maximum number of shader programs %d has been reached\n",UINT8_MAX);	
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
			app->programs[app->n_programs].handler,
			app->n_programs);
	*/
}
GLuint oglGetProg(oglApp *app, char *label){

	int i;
	for (i = 0; i < UINT8_MAX; ++i) {
		if (strcmp(app->programs[i].label, label) == 0) {
			return app->programs[i].handler;
		}		
	}	
	if (i == UINT8_MAX) {
		printf("ERROR: could not get %s shader program\n",label);
	}
	return 0;
}

void oglUseProg(oglApp *app, char *label){

	if (strcmp(app->c_program,label) != 0) {
		int i;
		for (i = 0; i < UINT8_MAX; ++i) {
			if (strcmp(app->programs[i].label, label) == 0) {
				glUseProgram(app->programs[i].handler);
				memset(app->c_program, 0, sizeof(uint8_t));
				strcpy(app->c_program, label);
				break;	
			}		
		}	
		if (i == UINT8_MAX) {
			printf("ERROR: could not find %s shader program\n",label);
		}

	}
}

void oglProgDelete(oglApp *app, char *label){

	int i;
	for (i = 0; i < UINT8_MAX; ++i) {
		if (strcmp(app->programs[i].label, label) == 0) {
			glDeleteProgram(app->programs[i].handler);
			break;	
		}		
	}	
	if (i == UINT8_MAX) {
		printf("ERROR: could not delete %s shader program\n",label);
	}

}


