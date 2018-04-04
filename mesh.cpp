#include <stdlib.h>
#include "mesh.h"

float rnd() {
	return 2.0f * float(rand()) / float(RAND_MAX) - 1.0f;
}

void insertModel(Mesh **list, int nv, float * vArr, int nt, int * tArr, float scale) {
	Mesh * mesh = (Mesh *) malloc(sizeof(Mesh));
	mesh->nv = nv;
	mesh->nt = nt;	
	mesh->vertices = (Vector *) malloc(nv * sizeof(Vector));
	mesh->vnorms = (Vector *)malloc(nv * sizeof(Vector));
	mesh->triangles = (Triangle *) malloc(nt * sizeof(Triangle));
	
	// set mesh vertices
	for (int i = 0; i < nv; i++) {
		mesh->vertices[i].x = vArr[i*3] * scale;
		mesh->vertices[i].y = vArr[i*3+1] * scale;
		mesh->vertices[i].z = vArr[i*3+2] * scale;
	}

	// set mesh triangles
	for (int i = 0; i < nt; i++) {
		mesh->triangles[i].vInds[0] = tArr[i*3];
		mesh->triangles[i].vInds[1] = tArr[i*3+1];
		mesh->triangles[i].vInds[2] = tArr[i*3+2];
	}

	// Assignment 1: 
	// Calculate and store suitable vertex normals for the mesh here.
	// Replace the code below that simply sets some arbitrary normal values	
	for (int i = 0; i < nv; i++) {
		mesh->vnorms[i].x = rnd();
		mesh->vnorms[i].y = rnd();
		mesh->vnorms[i].z = rnd();
	}

	// Set transform parameters
	mesh->translation = { 0, 0, 0 };
	mesh->rotation = { 0, 0, 0 };
	mesh->scale = { 1, 1, 1 };

	mesh->next = *list;
	*list = mesh;	
}

Matrix LocalToWorld(Vector t, Vector r, Vector s) {
	Matrix T = Translate(t.x, t.y, t.z);
	Matrix Rx = RotateX(r.x);
	Matrix Ry = RotateY(r.y);
	Matrix Rz = RotateZ(r.z);
	Matrix S = Scale(s.x, s.y, s.z);

	Matrix W = MatMatMul(T, MatMatMul(Rx, MatMatMul(Ry, MatMatMul(Rz, S))));
	return W;
}

//Kom ihåg att skicka tråden till GPU:n if mesh > cow
void RotateMesh(Mesh *mesh, float rot)
{
	Vector kossa;
	HomVector ret;
	Matrix m = RotateX(rot);

	for (int i = 0; i < mesh->nv; i++)
	{
		kossa.x = mesh->vertices[i].x;
		kossa.y = mesh->vertices[i].y;
		kossa.z = mesh->vertices[i].z;
		ret = MatVecMul(m, kossa);
		kossa = Homogenize(ret);
		mesh->vertices[i].x = kossa.x;
		mesh->vertices[i].y = kossa.y;
		mesh->vertices[i].z = kossa.z;

	}
}
