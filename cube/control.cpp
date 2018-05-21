#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#define GLEW_STATIC  
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <control.h>
#include <auxi.h>

#define SILENT -1
#define UPTURNING 1
#define LEFTTURNING 2
#define RIGHTTURNING 3
#define DOWNTURNING 4

extern GLFWwindow* window;
using namespace glm;

extern int width, height;

extern plane planes[6];
extern corner corners[8];
extern edge edges[12];

void fillInData();
void computeViewFromInputs();
void computePositionFromInputs();

mat4 getViewMatrix();

// Camera Attributes
vec3 position = vec4(0, 0, 10, 0);
vec3 right = vec4(1, 0, 0, 1);
vec3 up = vec4(0, 1, 0, 1);

mat4 view = lookAt(position, vec3(0, 0, 0), up);
mat4 getViewMatrix() {
	return view;
}

int Pcount;
int Vcount;
float deltaAngle = 0.15707963267948966192313216916398f;
int Vsitu = SILENT;
int Psitu = SILENT;
vec3 axis;
mat4 RotateMatrix;

int FRONT = 0;
int RIGHT = 1;
int BACK = 2;
int LEFT = 3;
int UP = 4;
int DOWN = 5;
//planes[0]//front
//planes[1]//right
//planes[2]//back
//planes[3]//left
//planes[4]//up
//planes[5]//down

bool normalwise = 0;

void UDturning() {
	if (Vcount < 10) {
		Vcount++;
		position = RotateMatrix * vec4(position.x, position.y, position.z, 0);
		up = RotateMatrix * vec4(up.x, up.y, up.z, 0);
		view = lookAt(position, vec3(0, 0, 0), up);
	}
	else {
		Vsitu = SILENT;
		Vcount = 0;
	}
}

void LRturning() {
	if (Vcount < 10) {
		Vcount++;
		position = RotateMatrix * vec4(position.x, position.y, position.z, 0);
		right = RotateMatrix * vec4(right.x, right.y, right.z, 0);
		up = cross(position, right);
		view = lookAt(position, vec3(0, 0, 0), up);
	}
	else {
		Vsitu = SILENT;
		Vcount = 0;
	}
}

void faceTurning() {
	if (Pcount < 10) {
		Pcount++;
		planes[Psitu].rotate(RotateMatrix);
		for (int i = 0; i < 8; i++)
			planes[Psitu].connects[i]->rotate(RotateMatrix);
		fillInData();
	}
	else {
		if (normalwise) {
			for (int i = 0; i < 4; i++) {
				planes[Psitu].connects[i]->precedent = planes[Psitu].connects[(i + 1) % 4];
				planes[Psitu].connects[i + 4]->precedent = planes[Psitu].connects[(i + 1) % 4 + 4];
			}
		}
		else {
			for (int i = 3; i >= 0; i--) {
				planes[Psitu].connects[(i + 1) % 4]->precedent = planes[Psitu].connects[i];
				planes[Psitu].connects[(i + 1) % 4 + 4]->precedent = planes[Psitu].connects[i + 4];
			}
		}

		RubikBlocks* memory[3];

		for (int i = 0; i < 2; i++) {
			memory[i] = planes[Psitu].connects[0]->connects[i];
		}
		RubikBlocks* temp = planes[Psitu].connects[0];

		for (int time = 0; time < 3; time++) {
			if (temp->precedent->connects[0] != &planes[Psitu]) {
				for (int j = 0; j < 8; j++) {
					if (temp->precedent->connects[0]->connects[j] == temp->precedent) {
						temp->precedent->connects[0]->connects[j] = temp;
						break;
					}
				}
			}
			else {
				for (int j = 0; j < 8; j++) {
					if (temp->precedent->connects[1]->connects[j] == temp->precedent) {
						temp->precedent->connects[1]->connects[j] = temp;
						break;
					}
				}
			}

			temp->connects[0] = temp->precedent->connects[0];
			temp->connects[1] = temp->precedent->connects[1];

			temp = temp->precedent;
		}
		if (memory[0] != &planes[Psitu]) {
			for (int j = 0; j < 8; j++) {
				if (memory[0]->connects[j] == temp->precedent) {
					memory[0]->connects[j] = temp;
					break;
				}
			}
		}
		else {
			for (int j = 0; j < 8; j++)
				if (memory[1]->connects[j] == temp->precedent) {
					memory[1]->connects[j] = temp;
					break;
				}
		}
		temp->connects[0] = memory[0];
		temp->connects[1] = memory[1];


		temp = planes[Psitu].connects[4];
		for (int i = 0; i < 3; i++)
			memory[i] = temp->connects[i];

		int rec[3];
		for (int i = 0; i < 3; i++)
			if (memory[i] != &planes[Psitu])
				for (int j = 0; j < 8; j++)
					if (memory[i]->connects[j] == temp) {
						rec[i] = j;
						break;
					}


		for (int time = 0; time < 3; time++) {
			for (int i = 0; i < 3; i++)
				if (temp->precedent->connects[i] != &planes[Psitu])
					for (int j = 0; j < 8; j++)
						if (temp->precedent->connects[i]->connects[j] == temp->precedent) {
							temp->precedent->connects[i]->connects[j] = temp;
							break;
						}


			temp->connects[0] = temp->precedent->connects[0];
			temp->connects[1] = temp->precedent->connects[1];
			temp->connects[2] = temp->precedent->connects[2];

			temp = temp->precedent;
		}

		for (int i = 0; i < 3; i++)
			if (memory[i] != &planes[Psitu])
				memory[i]->connects[rec[i]] = temp;

		temp->connects[0] = memory[0];
		temp->connects[1] = memory[1];
		temp->connects[2] = memory[2];

		Pcount = 0;
		Psitu = SILENT;
	}
}

void computeViewFromInputs() {
	if (Vsitu != SILENT) {
		if (Vsitu == UPTURNING || Vsitu == DOWNTURNING) {
			UDturning();
		}
		else LRturning();
	}
}

void computePositionFromInputs() {
	if (Psitu != SILENT)
		faceTurning();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS && key >= 262 && Vsitu == SILENT) {
		int temp = FRONT;
		switch (key) {
		case GLFW_KEY_UP:
			Vsitu = UPTURNING;

			FRONT = UP;
			UP = BACK;
			BACK = DOWN;
			DOWN = temp;

			RotateMatrix = rotate(deltaAngle, cross(position, up));
			UDturning();
			break;
		case GLFW_KEY_DOWN:
			Vsitu = DOWNTURNING;

			FRONT = DOWN;
			DOWN = BACK;
			BACK = UP;
			UP = temp;

			RotateMatrix = rotate(deltaAngle, cross(up, position));
			UDturning();
			break;
		case GLFW_KEY_LEFT:
			Vsitu = LEFTTURNING;

			FRONT = LEFT;
			LEFT = BACK;
			BACK = RIGHT;
			RIGHT = temp;

			RotateMatrix = rotate(deltaAngle, -up);
			LRturning();
			break;
		case GLFW_KEY_RIGHT:
			Vsitu = RIGHTTURNING;

			FRONT = RIGHT;
			RIGHT = BACK;
			BACK = LEFT;
			LEFT = temp;

			RotateMatrix = rotate(deltaAngle, up);
			LRturning();
			break;

		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		}
	}
	else if (action == GLFW_PRESS && Psitu == SILENT) {
		switch (key)
		{
		case GLFW_KEY_T:
			Psitu = UP;
			normalwise = 0;
			RotateMatrix = rotate(deltaAngle, -up);
			faceTurning();
			break;
		case GLFW_KEY_U:
			Psitu = UP;
			normalwise = 1;
			RotateMatrix = rotate(deltaAngle, up);
			faceTurning();
			break;
		case GLFW_KEY_F:
			Psitu = LEFT;
			normalwise = 0;
			RotateMatrix = rotate(deltaAngle, right);
			faceTurning();
			break;
		case GLFW_KEY_D:
			Psitu = LEFT;
			normalwise = 1;
			RotateMatrix = rotate(deltaAngle, -right);
			faceTurning();
			break;
		case GLFW_KEY_J:
			Psitu = RIGHT;
			normalwise = 1;
			RotateMatrix = rotate(deltaAngle, right);
			faceTurning();
			break;
		case GLFW_KEY_K:
			Psitu = RIGHT;
			normalwise = 0;
			RotateMatrix = rotate(deltaAngle, -right);
			faceTurning();
			break;
		case GLFW_KEY_V:
			Psitu = DOWN;
			normalwise = 1;
			RotateMatrix = rotate(deltaAngle, -up);
			faceTurning();
			break;
		case GLFW_KEY_N:
			Psitu = DOWN;
			normalwise = 0;
			RotateMatrix = rotate(deltaAngle, up);
			faceTurning();
			break;
		case GLFW_KEY_G:
			Psitu = FRONT;
			normalwise = 1;
			RotateMatrix = rotate(deltaAngle, cross(right, up));
			faceTurning();
			break;
		case GLFW_KEY_H:
			Psitu = FRONT;
			normalwise = 0;
			RotateMatrix = rotate(deltaAngle, -cross(right, up));
			faceTurning();
			break;
		default:
			break;
		}

	}
}