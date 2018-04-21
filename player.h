#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <glew.h>
#include <freeglut.h>
#include "mesh.h"

typedef struct _Camera {
	Vector position;
	Vector rotation;
	double fov;
	double nearPlane;
	double farPlane;
} Camera;

typedef struct _Shader {
	GLuint program;
	const char * shaderFiles[2];
	_Shader *next;
} Shader;

typedef struct _Player {
	Camera *cam;
	int screen_width, screen_height;
	_Shader *shader;
	int shaderMode = 0, viewMode = 0, projMode = 0, passMouse = 0;
} Player;

extern Player *player;

void renderMesh(Mesh *mesh, Matrix V, Matrix P, Matrix PV);
void display(void);
#endif