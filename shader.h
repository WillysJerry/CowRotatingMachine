#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_

#include <stdio.h>
#include <glew.h>
#include <freeglut.h>
#include "algebra.h"
#include "light.h"
#include "mesh.h"
#include "player.h"

void changeShader(void);
GLuint prepareShaderProgram(const char ** vs_src, const char ** fs_src);
void prepareMesh(Mesh *mesh, GLuint shader);
void readShaderFile(const char file[], const char *shader[]);
void renderMesh(Mesh *mesh, Matrix V, Matrix P, Matrix PV);

#endif
