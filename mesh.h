#ifndef _MESH_H_
#define _MESH_H_

#include "algebra.h"

typedef struct _Triangle {
	int vInds[3]; //vertex indices
} Triangle;

typedef struct _Mesh { 
	int nv;				
	Vector *vertices;
	Vector *vnorms;
	int nt;				
	Triangle *triangles;
	struct _Mesh *next; 
		
	unsigned int vbo, ibo, vao; // OpenGL handles for rendering

	Vector translation;
	Vector rotation;
	Vector scale;
} Mesh;

typedef struct _Camera {
	Vector position;
	Vector rotation;
	double fov; 
	double nearPlane; 
	double farPlane; 
} Camera;

void insertModel(Mesh ** objlist, int nv, float * vArr, int nt, int * tArr, float scale = 1.0);

void RotateMesh(Mesh *mesh, float rot);

Matrix LocalToWorld(Vector t, Vector r, Vector s);
#endif
