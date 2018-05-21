#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <auxi.h>
using namespace glm;

plane planes[6];
corner corners[8];
edge edges[12];
midFace mids[6];
face faces[48];
GLfloat blockVertex_data[972];
GLfloat color_buffer_data[972];

float pi2 = 1.5707963267948966192313216916398f;

void fillInData() {
	int p = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 18; j++) {
			blockVertex_data[p] = planes[i].vertex_buffer_data[j];
			p++;
		}
	}

	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 36; j++) {
			blockVertex_data[p] = edges[i].vertex_buffer_data[j];
			p++;
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 54; j++) {
			blockVertex_data[p] = corners[i].vertex_buffer_data[j];
			p++;
		}
	}
}

GLfloat colors[6][3] = {
	{ 1.0f, 0.0f, 0.0f },//red front
{ 0.0f, 0.6f, 0.0f },//green right
{ 1.0f, 0.5f, 0.15f },//orange back
{ 0.0f, 0.0f, 1.0f },//blue left
{ 1.0f, 1.0f, 0.0f },//yellow up
{ 1.0f, 1.0f, 1.0f },//white down
};

int p = 0;
#define front 0
#define right 1
#define back 2
#define left 3
#define up 4
#define down 5

void colorFace(int i) {
	for (int j = 0; j < 6; j++) {
		color_buffer_data[p] = colors[i][0];
		color_buffer_data[p + 1] = colors[i][1];
		color_buffer_data[p + 2] = colors[i][2];
		p += 3;
	}
}// p+=3*6 = 18. A face -> 6point -> 18data

void colorInit() {
	//planes
	for (int i = 0; i < 6; i++) {
		colorFace(i);
	}//18*6 = 108

	 //edges
	colorFace(front);
	colorFace(up);
	colorFace(front);
	colorFace(right);
	colorFace(front);
	colorFace(down);
	colorFace(front);
	colorFace(left);

	colorFace(back);
	colorFace(up);
	colorFace(back);
	colorFace(right);
	colorFace(back);
	colorFace(down);
	colorFace(back);
	colorFace(left);

	colorFace(right);
	colorFace(up);
	colorFace(down);
	colorFace(right);
	colorFace(left);
	colorFace(down);
	colorFace(up);
	colorFace(left);
	//2*18*4*3 = 432

	colorFace(front);
	colorFace(up);
	colorFace(left);
	colorFace(front);
	colorFace(right);
	colorFace(up);
	colorFace(front);
	colorFace(down);
	colorFace(right);
	colorFace(front);
	colorFace(left);
	colorFace(down);

	colorFace(back);
	colorFace(up);
	colorFace(left);
	colorFace(back);
	colorFace(right);
	colorFace(up);
	colorFace(back);
	colorFace(down);
	colorFace(right);
	colorFace(back);
	colorFace(left);
	colorFace(down);
	//3*4*2*18 = 432
}

void RubikInit() {
	planes[0].position = vec3(0, 0, 1.5);//front
	planes[1].position = vec3(1.5, 0, 0);//right
	planes[2].position = vec3(0, 0, -1.5);//back
	planes[3].position = vec3(-1.5, 0, 0);//left
	planes[4].position = vec3(0, 1.5, 0);//up
	planes[5].position = vec3(0, -1.5, 0);//down

	GLfloat temp1[] = { 0.5f, 0.5f, 1.5f,
		0.5f, -0.5f, 1.5f,
		-0.5f, 0.5f, 1.5f,
		-0.5f, -0.5f, 1.5f,
		0.5f, -0.5f, 1.5f,
		-0.5f, 0.5f, 1.5f };

	for (int i = 0; i < 18; i++)
		planes[0].vertex_buffer_data[i] = temp1[i];

	vec3 axis;
	mat4 RotateMatrix;

	axis = vec3(0, 1, 0);
	RotateMatrix = rotate(pi2, axis);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 6; j++) {
			vec3 temp = RotateMatrix * vec4(planes[i].vertex_buffer_data[j * 3], planes[i].vertex_buffer_data[j * 3 + 1], planes[i].vertex_buffer_data[j * 3 + 2], 1);
			planes[i + 1].vertex_buffer_data[j * 3] = temp.x;
			planes[i + 1].vertex_buffer_data[j * 3 + 1] = temp.y;
			planes[i + 1].vertex_buffer_data[j * 3 + 2] = temp.z;
		}
	}

	axis = vec3(-1, 0, 0);
	RotateMatrix = rotate(pi2, axis);
	for (int i = 4; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			vec3 temp = RotateMatrix * vec4(planes[0].vertex_buffer_data[j * 3], planes[0].vertex_buffer_data[j * 3 + 1], planes[0].vertex_buffer_data[j * 3 + 2], 1);
			planes[i].vertex_buffer_data[j * 3] = temp.x;
			planes[i].vertex_buffer_data[j * 3 + 1] = temp.y;
			planes[i].vertex_buffer_data[j * 3 + 2] = temp.z;
		}
		RotateMatrix = rotate(3 * pi2, axis);
	}

	////////////////////////////////////////////////////////////

	GLfloat temp2[] =
	{ 0.5f, 1.5f, 1.5f,
		0.5f, 0.5f, 1.5f,
		-0.5f, 1.5f, 1.5f,

		-0.5f, 0.5f, 1.5f,
		0.5f, 0.5f, 1.5f,
		-0.5f, 1.5f, 1.5f,

		-0.5f, 1.5f, 1.5f,
		0.5f, 1.5f, 1.5f,
		-0.5f, 1.5f, 0.5f,

		0.5f, 1.5f, 0.5f,
		0.5f, 1.5f, 1.5f,
		-0.5f, 1.5f, 0.5f,
	};

	for (int i = 0; i < 36; i++)
		edges[0].vertex_buffer_data[i] = temp2[i];

	axis = vec3(0, 0, -1);
	RotateMatrix = rotate(pi2, axis);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 12; j++) {
			vec3 temp = RotateMatrix * vec4(edges[i].vertex_buffer_data[j * 3], edges[i].vertex_buffer_data[j * 3 + 1], edges[i].vertex_buffer_data[j * 3 + 2], 1);
			edges[i + 1].vertex_buffer_data[j * 3] = temp.x;
			edges[i + 1].vertex_buffer_data[j * 3 + 1] = temp.y;
			edges[i + 1].vertex_buffer_data[j * 3 + 2] = temp.z;
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 12; j++) {
			edges[i + 4].vertex_buffer_data[j * 3] = edges[i].vertex_buffer_data[j * 3];
			edges[i + 4].vertex_buffer_data[j * 3 + 1] = edges[i].vertex_buffer_data[j * 3 + 1];
			edges[i + 4].vertex_buffer_data[j * 3 + 2] = -edges[i].vertex_buffer_data[j * 3 + 2];
		}
	}

	axis = vec3(0, 1, 0);
	RotateMatrix = rotate(pi2, axis);
	for (int j = 0; j < 12; j++) {
		vec3 temp = RotateMatrix * vec4(edges[0].vertex_buffer_data[j * 3], edges[0].vertex_buffer_data[j * 3 + 1], edges[0].vertex_buffer_data[j * 3 + 2], 1);
		edges[8].vertex_buffer_data[j * 3] = temp.x;
		edges[8].vertex_buffer_data[j * 3 + 1] = temp.y;
		edges[8].vertex_buffer_data[j * 3 + 2] = temp.z;
	}

	axis = vec3(0, 0, -1);
	RotateMatrix = rotate(pi2, axis);
	for (int i = 8; i < 11; i++) {
		for (int j = 0; j < 12; j++) {
			vec3 temp = RotateMatrix * vec4(edges[i].vertex_buffer_data[j * 3], edges[i].vertex_buffer_data[j * 3 + 1], edges[i].vertex_buffer_data[j * 3 + 2], 1);
			edges[i + 1].vertex_buffer_data[j * 3] = temp.x;
			edges[i + 1].vertex_buffer_data[j * 3 + 1] = temp.y;
			edges[i + 1].vertex_buffer_data[j * 3 + 2] = temp.z;
		}
	}

	////////////////////////////////////////////////////////////
	//planes[0]//front
	//planes[1]//right
	//planes[2]//back
	//planes[3]//left
	//planes[4]//up
	//planes[5]//down

	planes[0].connects[0] = &edges[0];
	planes[0].connects[1] = &edges[3];
	planes[0].connects[2] = &edges[2];
	planes[0].connects[3] = &edges[1];

	planes[1].connects[0] = &edges[1];
	planes[1].connects[1] = &edges[9];
	planes[1].connects[2] = &edges[5];
	planes[1].connects[3] = &edges[8];

	planes[2].connects[0] = &edges[4];
	planes[2].connects[1] = &edges[5];
	planes[2].connects[2] = &edges[6];
	planes[2].connects[3] = &edges[7];

	planes[3].connects[0] = &edges[11];
	planes[3].connects[1] = &edges[7];
	planes[3].connects[2] = &edges[10];
	planes[3].connects[3] = &edges[3];

	planes[4].connects[0] = &edges[0];
	planes[4].connects[1] = &edges[8];
	planes[4].connects[2] = &edges[4];
	planes[4].connects[3] = &edges[11];

	planes[5].connects[0] = &edges[2];
	planes[5].connects[1] = &edges[10];
	planes[5].connects[2] = &edges[6];
	planes[5].connects[3] = &edges[9];

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			if (planes[i].connects[j]->connects[0] == NULL)
				planes[i].connects[j]->connects[0] = &planes[i];
			else planes[i].connects[j]->connects[1] = &planes[i];
		}
	}
	////////////////////////////////////////////////////////////

	GLfloat temp3[] =
	{ -0.5f, 1.5f, 1.5f,
		-0.5f, 0.5f, 1.5f,
		-1.5f, 1.5f, 1.5f,
		-1.5f, 0.5f, 1.5f,
		-0.5f, 0.5f, 1.5f,
		-1.5f, 1.5f, 1.5f,

		-1.5f, 1.5f, 1.5f,
		-0.5f, 1.5f, 1.5f,
		-1.5f, 1.5f, 0.5f,
		-0.5f, 1.5f, 0.5f,
		-0.5f, 1.5f, 1.5f,
		-1.5f, 1.5f, 0.5f,

		-1.5f, 1.5f, 1.5f,
		-1.5f, 0.5f, 1.5f,
		-1.5f, 0.5f, 0.5f,
		-1.5f, 1.5f, 1.5f,
		-1.5f, 1.5f, 0.5f,
		-1.5f, 0.5f, 0.5f,
	};

	for (int i = 0; i < 54; i++)
		corners[0].vertex_buffer_data[i] = temp3[i];

	axis = vec3(0, 0, -1);
	RotateMatrix = rotate(pi2, axis);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 18; j++) {
			vec3 temp = RotateMatrix * vec4(corners[i].vertex_buffer_data[j * 3], corners[i].vertex_buffer_data[j * 3 + 1], corners[i].vertex_buffer_data[j * 3 + 2], 1);
			corners[i + 1].vertex_buffer_data[j * 3] = temp.x;
			corners[i + 1].vertex_buffer_data[j * 3 + 1] = temp.y;
			corners[i + 1].vertex_buffer_data[j * 3 + 2] = temp.z;
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 18; j++) {
			corners[i + 4].vertex_buffer_data[j * 3] = corners[i].vertex_buffer_data[j * 3];
			corners[i + 4].vertex_buffer_data[j * 3 + 1] = corners[i].vertex_buffer_data[j * 3 + 1];
			corners[i + 4].vertex_buffer_data[j * 3 + 2] = -corners[i].vertex_buffer_data[j * 3 + 2];
		}
	}

	////////////////////////////////////////////////////////////
	//planes[0]//front
	//planes[1]//right
	//planes[2]//back
	//planes[3]//left
	//planes[4]//up
	//planes[5]//down

	planes[0].connects[4] = &corners[0];
	planes[0].connects[5] = &corners[3];
	planes[0].connects[6] = &corners[2];
	planes[0].connects[7] = &corners[1];

	planes[1].connects[4] = &corners[1];
	planes[1].connects[5] = &corners[2];
	planes[1].connects[6] = &corners[6];
	planes[1].connects[7] = &corners[5];

	planes[2].connects[4] = &corners[4];
	planes[2].connects[5] = &corners[5];
	planes[2].connects[6] = &corners[6];
	planes[2].connects[7] = &corners[7];

	planes[3].connects[4] = &corners[0];
	planes[3].connects[5] = &corners[4];
	planes[3].connects[6] = &corners[7];
	planes[3].connects[7] = &corners[3];

	planes[4].connects[4] = &corners[0];
	planes[4].connects[5] = &corners[1];
	planes[4].connects[6] = &corners[5];
	planes[4].connects[7] = &corners[4];

	planes[5].connects[4] = &corners[2];
	planes[5].connects[5] = &corners[3];
	planes[5].connects[6] = &corners[7];
	planes[5].connects[7] = &corners[6];

	for (int i = 0; i < 6; i++) {
		for (int j = 4; j < 8; j++) {
			if (planes[i].connects[j]->connects[0] == NULL)
				planes[i].connects[j]->connects[0] = &planes[i];
			else if (planes[i].connects[j]->connects[1] == NULL)
				planes[i].connects[j]->connects[1] = &planes[i];
			else planes[i].connects[j]->connects[2] = &planes[i];
		}
	}
	////////////////////////////////////////////////////////////

}

void faceInit() {
	GLfloat temp1[] = { 0.5f, 0.5f, 1.5f,
		0.5f, -0.5f, 1.5f,
		-0.5f, 0.5f, 1.5f,
		-0.5f, -0.5f, 1.5f,
		0.5f, -0.5f, 1.5f,
		-0.5f, 0.5f, 1.5f };

	for (int i = 0; i < 18; i++) {
		mids[0].vertex_buffer_data[i] = temp1[i];
	}
	//planes[0]//front
	//planes[1]//right
	//planes[2]//back
	//planes[3]//left
	//planes[4]//up
	//planes[5]//down

	vec3 axis;
	mat4 RotateMatrix;

	axis = vec3(0, 1, 0);
	RotateMatrix = rotate(pi2, axis);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 6; j++) {
			vec3 temp = RotateMatrix * vec4(mids[i].vertex_buffer_data[j * 3], mids[i].vertex_buffer_data[j * 3 + 1], mids[i].vertex_buffer_data[j * 3 + 2], 1);
			mids[i + 1].vertex_buffer_data[i * 3] = temp.x;
			mids[i + 1].vertex_buffer_data[i * 3 + 1] = temp.y;
			mids[i + 1].vertex_buffer_data[i * 3 + 2] = temp.z;
		}
	}

	axis = vec3(-1, 0, 0);
	RotateMatrix = rotate(pi2, axis);
	for (int i = 4; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			vec3 temp = RotateMatrix * vec4(mids[0].vertex_buffer_data[j * 3], mids[0].vertex_buffer_data[j * 3 + 1], mids[0].vertex_buffer_data[j * 3 + 2], 1);
			mids[i].vertex_buffer_data[i * 3] = temp.x;
			mids[i].vertex_buffer_data[i * 3 + 1] = temp.y;
			mids[i].vertex_buffer_data[i * 3 + 2] = temp.z;
		}
		RotateMatrix = rotate(3 * pi2, axis);
	}
}