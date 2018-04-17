#ifndef _MESH_H_
#define _MESH_H_

#include "algebra.h"

typedef struct _Triangle {
	int vInds[3];
} Triangle;

typedef struct _Mesh { 
	int nv;				
	Vector *vertices;
	Vector *vnorms;
	int nt;				
	Triangle *triangles;
	struct _Mesh *next; 
		
	Vector translation;
	Vector rotation;
	Vector scale;
	unsigned int vbo, ibo, vao;
} Mesh;

typedef struct _Camera {
	Vector position;
	Vector rotation;
	double fov; 
	double nearPlane; 
	double farPlane;
	Vector eye;
	Vector up;
	Vector center;
} Camera;

void insertModel(Mesh ** objlist, int nv, float * vArr, int nt, int * tArr, float scale = 1.0);

Matrix LocalToWorld(Vector t, Vector r, Vector s);
int insertModelFromFile(Mesh **list, const char* filename);
#endif
