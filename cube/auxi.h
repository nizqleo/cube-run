#pragma once
#pragma once
#ifndef CIRCLE_H
#define CIRCLE_H
using namespace glm;

class RubikBlocks {
public:
	GLfloat vertex_buffer_data[54];
	RubikBlocks* connects[8];
	RubikBlocks* precedent;
	virtual void rotate(mat4 RotateMatrix) {}
};

class plane : public RubikBlocks {// 18
public:
	vec3 position;

	plane() {}
	void rotate(mat4 RotateMatrix) {
		for (int i = 0; i < 6; i++) {
			vec4 temp = RotateMatrix * vec4(vertex_buffer_data[i * 3], vertex_buffer_data[i * 3 + 1], vertex_buffer_data[i * 3 + 2], 1);
			vertex_buffer_data[i * 3] = temp.x;
			vertex_buffer_data[i * 3 + 1] = temp.y;
			vertex_buffer_data[i * 3 + 2] = temp.z;
		}
	}
};

class edge :public RubikBlocks {// 36
public:
	edge() {}
	void rotate(mat4 RotateMatrix) {
		for (int i = 0; i < 12; i++) {
			vec4 temp = RotateMatrix * vec4(vertex_buffer_data[i * 3], vertex_buffer_data[i * 3 + 1], vertex_buffer_data[i * 3 + 2], 1);
			vertex_buffer_data[i * 3] = temp.x;
			vertex_buffer_data[i * 3 + 1] = temp.y;
			vertex_buffer_data[i * 3 + 2] = temp.z;
		}
	}
};

class corner :public RubikBlocks {// 54
public:
	corner() {}
	void rotate(mat4 RotateMatrix) {
		for (int i = 0; i < 18; i++) {
			vec4 temp = RotateMatrix * vec4(vertex_buffer_data[i * 3], vertex_buffer_data[i * 3 + 1], vertex_buffer_data[i * 3 + 2], 1);
			vertex_buffer_data[i * 3] = temp.x;
			vertex_buffer_data[i * 3 + 1] = temp.y;
			vertex_buffer_data[i * 3 + 2] = temp.z;
		}
	}
};

class face {
public:
	GLfloat vertex_buffer_data[18];
};

class midFace :public face {
public:
	face * cross[4];
	face* corner[4];
	face* downX[4];
	face* downCorL[4];
	face* downCorR[4];
};


#endif