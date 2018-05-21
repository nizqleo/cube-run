#include <stdio.h>
#include <stdlib.h>

#include "gl_env.h"

GLFWwindow* window;

#include <glm/glm.hpp>
using namespace glm;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "control.h"

int width, height;
extern GLfloat blockVertex_data[972];
extern GLfloat color_buffer_data[972];
void RubikInit();
void faceInit();
void fillInData();
void colorInit();
extern void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
extern mat4 getViewMatrix();
extern void computeViewFromInputs();
extern void computePositionFromInputs();

int main(void)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	height = 1080;
	width = 1920;

	window = glfwCreateWindow(height, width, "Rubik", NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.4f, 0.0f, 0.0f);

	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	////faceInit();
	RubikInit();
	colorInit();
	fillInData();

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint blockVertex;
	glGenBuffers(1, &blockVertex);
	glBindBuffer(GL_ARRAY_BUFFER, blockVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blockVertex_data), blockVertex_data, GL_DYNAMIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

	glfwGetFramebufferSize(window, &width, &height);

	mat4 Projection = perspective(radians(45.0f), 1.0f, 0.1f, 100.0f);
	mat4 Model = mat4(1.0f);

	mat4 ViewMatrix = getViewMatrix();

	GLuint programID = LoadShaders("vs1.vs", "fs1.fsd");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glfwSetKeyCallback(window, key_callback);

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		computeViewFromInputs();
		mat4 ViewMatrix = getViewMatrix();
		mat4 MVP = Projection * ViewMatrix * Model;

		glUseProgram(programID);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		computePositionFromInputs();
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, blockVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(blockVertex_data), blockVertex_data, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 324); // Starting from vertex 0; 3 vertices total -> 1 triangle

		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwWindowShouldClose(window) == 0);

	glfwTerminate();

	return 0;
}