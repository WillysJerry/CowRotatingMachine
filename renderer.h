#pragma once
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <glew.h>
#include <freeglut.h>
#include <math.h>
#include "mesh.h"
#include "light.h"

typedef struct {
	Camera* cam;
	int screen_width, screen_height;
	Mesh* meshList;
	PointLight* light;
	GLuint shprg; // Shader program id MAYBE REMOVE THIS LATER
	int shaderMode = 0, viewMode = 0, projMode = 0;

} Renderer;

Renderer initRenderer(Camera* cam, int screen_width, int screen_height, Mesh* meshList, PointLight* light, GLuint shprg);
void setActiveRenderer(Renderer* renderer);
void renderMesh(Mesh *mesh, Matrix V, Matrix P, Matrix PV);
void display(void);
#endif