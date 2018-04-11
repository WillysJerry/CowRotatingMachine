#define _USE_MATH_DEFINES // To get M_PI defined
#include <math.h>
#include <stdio.h>
//#include "mesh.h"
#include "algebra.h"
#include <string.h>

Vector CrossProduct(Vector a, Vector b) {
	Vector v = { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
	return v;
}

float DotProduct(Vector a, Vector b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector Subtract(Vector a, Vector b) {
	Vector v = { a.x-b.x, a.y-b.y, a.z-b.z };
	return v;
}    

Vector Add(Vector a, Vector b) {
	Vector v = { a.x+b.x, a.y+b.y, a.z+b.z };
	return v;
}    

float Length(Vector a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

Vector Normalize(Vector a) {
	float len = Length(a);
	Vector v = { a.x/len, a.y/len, a.z/len };
	return v;
}

Vector ScalarVecMul(float t, Vector a) {
	Vector b = { t*a.x, t*a.y, t*a.z };
	return b;
}

HomVector MatVecMul(Matrix a, Vector b) {
	HomVector h;
	h.x = b.x*a.e[0] + b.y*a.e[4] + b.z*a.e[8] + a.e[12];
	h.y = b.x*a.e[1] + b.y*a.e[5] + b.z*a.e[9] + a.e[13];
	h.z = b.x*a.e[2] + b.y*a.e[6] + b.z*a.e[10] + a.e[14];
	h.w = b.x*a.e[3] + b.y*a.e[7] + b.z*a.e[11] + a.e[15];
	return h;
}

Vector Homogenize(HomVector h) {
	Vector a;
	if (h.w == 0.0) {
		fprintf(stderr, "Homogenize: w = 0\n");
		a.x = a.y = a.z = 9999999;
		return a;
	}
	a.x = h.x / h.w;
	a.y = h.y / h.w;
	a.z = h.z / h.w;
	return a;
}

Matrix MatMatMul(Matrix a, Matrix b) {
	Matrix c;
	int i, j, k;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			c.e[j*4+i] = 0.0;
			for (k = 0; k < 4; k++)
				c.e[j*4+i] += a.e[k*4+i] * b.e[j*4+k];
		}
	}
	return c;
}

void PrintVector(char *name, Vector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z);
}

void PrintHomVector(char *name, HomVector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z, a.w);
}

void PrintMatrix(char *name, Matrix a) { 
	int i,j;

	printf("%s:\n", name);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("%6.5lf ", a.e[j*4+i]);
		}
		printf("\n");
	}
}


Matrix Translate(float x, float y, float z)
{
	Matrix m;
	float arr[16] = { 1,0,0,0,0,1,0,0,0,0,1,0,x,y,z,1 };
	memcpy(m.e, arr, 16 * sizeof(float));
	return m;
}

Matrix Scale(float x, float y, float z)
{
	Matrix m;
	float arr[16] = { x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1 };
	memcpy(m.e, arr, 16 * sizeof(float));
	return m;
}

Matrix RotateX(float theta)
{
	Matrix m;
	float arr[16] = { 1,0,0,0,0, cos(theta), sin(theta), 0, 0, -sin(theta), cos(theta), 0,0,0,0,1 };
	memcpy(m.e, arr, 16 * sizeof(float));
	return m;
}

Matrix RotateY(float theta)
{
	Matrix m;
	float arr[16] = { cos(theta), 0, -sin(theta), 0,0,1,0,0, sin(theta), 0, cos(theta), 0, 0, 0, 0, 1 };
	memcpy(m.e, arr, 16 * sizeof(float));
	return m;
}

Matrix RotateZ(float theta)
{
	Matrix m;
	float arr[16] = { cos(theta), sin(theta), 0,0, -sin(theta), cos(theta), 0,0,0,0,1,0,0,0,0,1 };
	memcpy(m.e, arr, 16 * sizeof(float));
	return m;
}

Matrix MatOrtho(float left, float right, float bottom, float top, float near, float far) {
	// Orthogonal projektion:
	// Föreläsning 2.5 Viewing - s.15
	Matrix m;

	float arr[16] = {
		2 / (right - left), 0, 0, 0,
		0, 2 / (top - bottom), 0, 0,
		0, 0, 2 / (near - far), 0,
		-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -((far + near) / (far - near)), 1
	};

	memcpy(m.e, arr, 16 * sizeof(float));
	return m;
}

Matrix MatPerspective(float fovy, float aspect, float near, float far) {
	Matrix m;
	float arr[16] = {
		(1 / tan(fovy / 2.0)) / aspect, 0, 0, 0,
		0, 1 / tan(fovy / 2.0), 0, 0,
		0, 0, (far + near) / (far - near), (2.0 * far * near) / (near - far),
		0, 0, -1, 0
	};
	memcpy(m.e, arr, 16 * sizeof(float));
	return m;
}

Matrix MatFrustum(float left, float right, float bottom, float top, float near, float far) {
	// Frustum perspective projektion:
	// Föreläsning 2.5 Viewing - s.21
	Matrix m;
	float arr[16] = {
		2 * near / (right - left), 0, 0, 0,
		0, 2 * near / (top - bottom), 0, 0,
		(right + left) / (right - left), (top + bottom) / (top - bottom), -((far + near) / (far - near)), -1,
		0, 0, -(2 * far * near / (far - near)), 0
	};

	memcpy(m.e, arr, 16 * sizeof(float));
	return m;
}

Matrix Bounce(float a, float t) {
	return Translate(0, sin(t) * a, 0);
}

//The lookX/Y/Z vectors are the sum of PosX + LookAtX etc.
Matrix matLookAt(Vector pos, Vector look, Vector up)
{

}
