#ifndef _ALGEBRA_H_
#define _ALGEBRA_H_

typedef struct { float x, y, z; } Vector;
typedef struct { float x, y, z, w; } HomVector;

/* Column-major order are used for the matrices here to be compatible with OpenGL.
** The indices used to access elements in the matrices are shown below.
**  _                _
** |                  |
** |   0   4   8  12  |
** |                  |
** |   1   5   9  13  |
** |                  |
** |   2   6  10  14  |
** |                  |
** |   3   7  11  15  |
** |_                _|
							** Translate-matris!?
							**  _                _
							** |                  |
							** |   1   0   0   x  |
							** |                  |
							** |   0   1   0   y  |
							** |                  |
							** |   0   0   1   z  |
							** |                  |
							** |   0   0   0   1  |
							** |_                _|
*/
typedef struct matrix { float e[16]; } Matrix;

Vector Add(Vector a, Vector b);
Vector Subtract(Vector a, Vector b);
Vector CrossProduct(Vector a, Vector b);
float DotProduct(Vector a, Vector b);
float Length(Vector a);
Vector Normalize(Vector a);
Vector ScalarVecMul(float t, Vector a);
HomVector MatVecMul(Matrix a, Vector b);
Vector Homogenize(HomVector a);
Matrix MatMatMul(Matrix a, Matrix b);
void PrintMatrix(char *name, Matrix m);
void PrintVector(char *name, Vector v);
void PrintHomVector(char *name, HomVector h);
Matrix Translate(float x, float y, float z);
Matrix Scale(float x, float y, float z);
Matrix RotateX(float theta);
Matrix RotateY(float theta);
Matrix RotateZ(float theta);
//void RotateMesh(Mesh *mesh, float rot);
Matrix MatOrtho(float left, float right, float bottom, float top, float nearPlane, float farPlane);
Matrix MatPerspective(float fovy, float aspect, float nearPlane, float farPlane);
Matrix MatFrustum(float left, float right, float bottom, float top, float nearPlane, float farPlane);

float Deg2Rad(float deg);
Matrix Bounce(float a, float t);
#endif

