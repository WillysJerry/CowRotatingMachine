//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <glew.h>
#include <freeglut.h>
#include <math.h>
#include "mesh.h"

#include "algebra.h"
#include "shaders.h"

int bounceMode = 0;// Toggle between predefined or dynamic perspective matrix
int viewMode = 0;// Toggle between predefined or dynamic perspective matrix
int shaderMode = 0;// Turn of faces

int screen_width = 1024;
int screen_height = 768;

Mesh *meshList = NULL; // Global pointer to linked list of triangle meshes
Mesh *selected = NULL;

Camera cam = {{0,0,20}, {0,0,0}, 60, 1, 10000}; // Setup the global camera parameters, i OpenGL s� tittar kameran "bak�t" s� +20 z-axis anv�nds f�r att f� lite avst�nd till modellen.

GLuint shprg; // Shader program id


// Global transform matrices
// V is the view transform
// P is the projection transform
// PV = P * V is the combined view-projection transform
Matrix V, P, PV;
Matrix M;

float t = 0; // Temporary time variable for mesh bounce animation

void prepareShaderProgram(const char ** vs_src, const char ** fs_src) {
	GLint success = GL_FALSE;

	shprg = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_src, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);	
	if (!success) printf("Error in vertex shader!\n");
	else printf("Vertex shader compiled successfully!\n");

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_src, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);	
	if (!success) printf("Error in fragment shader!\n");
	else printf("Fragment shader compiled successfully!\n");

	glAttachShader(shprg, vs);
	glAttachShader(shprg, fs);
	glLinkProgram(shprg);
	GLint isLinked = GL_FALSE;
	glGetProgramiv(shprg, GL_LINK_STATUS, &isLinked);
	if (!isLinked) printf("Link error in shader program!\n");
	else printf("Shader program linked successfully!\n");
}

void prepareMesh(Mesh *mesh) {
	int sizeVerts = mesh->nv * 3 * sizeof(float);
	int sizeCols  = mesh->nv * 3 * sizeof(float);
	int sizeTris = mesh->nt * 3 * sizeof(int);
	
	// For storage of state and other buffer objects needed for vertex specification
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	// Allocate VBO and load mesh data (vertices and normals)
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeVerts + sizeCols, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVerts, (void *)mesh->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeVerts, sizeCols, (void *)mesh->vnorms);

	// Allocate index buffer and load mesh indices
	glGenBuffers(1, &mesh->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeTris, (void *)mesh->triangles, GL_STATIC_DRAW);

	// Define the format of the vertex data
	GLint vPos = glGetAttribLocation(shprg, "vPos");
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Define the format of the vertex data 
	GLint vNorm = glGetAttribLocation(shprg, "vNorm");
	glEnableVertexAttribArray(vNorm);
	glVertexAttribPointer(vNorm, 3, GL_FLOAT, GL_FALSE, 0, (void *)(mesh->nv * 3 *sizeof(float)));

	glBindVertexArray(0);

}

void renderMesh(Mesh *mesh) {
	
	// Assignment 1: Apply the transforms from local mesh coordinates to world coordinates here
	// Combine it with the viewing transform that is passed to the shader below
	Matrix W = MatMatMul(Bounce(2, t), LocalToWorld(mesh->translation, mesh->rotation, mesh->scale));
	if (bounceMode == 1) {
		t += 0.05f; // Removing this will stop the bouncing animation
	}

	M = MatMatMul(PV, W);

	// TEMP TEST ROTATION AND TRANSLATION
	//mesh->translation.x += 0.1f;
	//mesh->rotation.y += 0.1f;

	// Pass the viewing transform to the shader
	//GLint loc_PV = glGetUniformLocation(shprg, "PV");
	//glUniformMatrix4fv(loc_PV, 1, GL_FALSE, PV.e);
	GLint loc_PV = glGetUniformLocation(shprg, "PV");
	glUniformMatrix4fv(loc_PV, 1, GL_FALSE, M.e);


	// Select current resources 
	glBindVertexArray(mesh->vao);
	
	// To accomplish wireframe rendering (can be removed to get filled triangles)
	if (shaderMode == 1) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Draw all triangles
	glDrawElements(GL_TRIANGLES, mesh->nt * 3, GL_UNSIGNED_INT, NULL); 
	glBindVertexArray(0);
}


void display(void) {
	Mesh *mesh;
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); //Lade till DEPTH_BUFFER f�r att kunna rita upp polygoner

	// Assignment 1: Calculate the transform to view coordinates yourself 	
	// The matrix V should be calculated from camera parameters
	// Therefore, you need to replace this hard-coded transform. 

	float a = cam.rotation.x, b = cam.rotation.y, c = cam.rotation.z;
	Vector pos = { -cam.position.x, -cam.position.y, -cam.position.z };

	Matrix rz = RotateZ(-c);
	Matrix ry = RotateY(-b);
	Matrix rx = RotateX(-a);
	Matrix t = Translate(pos.x, pos.y, pos.z);

	V = MatMatMul(t, MatMatMul(rx, MatMatMul(rz, ry)));

	// Assignment 1: Calculate the projection transform yourself 	
	// The matrix P should be calculated from camera parameters
	// Therefore, you need to replace this hard-coded transform. 	
	
	if (viewMode == 0) {
		P = MatOrtho(-10, 10, -10, 10, 0, 100000);
	}
	else if (viewMode == 1){
		P = MatPerspective(45, screen_width / screen_height, 1, 100000);
	}
	else {
		P = MatFrustum(-1, 1, -1, 1, 1, 100000);

	}

	// This finds the combined view-projection matrix
	PV = MatMatMul(P, V);


	// Select the shader program to be used during rendering 
	glUseProgram(shprg);

	// Render all meshes in the scene
	mesh = meshList;
		
	while (mesh != NULL) {
		renderMesh(mesh);
		mesh = mesh->next;
	}

	glFlush();
}

void changeSize(int w, int h) {
	screen_width = w;
	screen_height = h;
	glViewport(0, 0, screen_width, screen_height);

}

void CameraSettings() {
	printf("Position: (%2f, %2f, %2f)\n", cam.position.x, cam.position.y, cam.position.z);
	printf("Rotation: (%2f, %2f, %2f)\n", cam.rotation.x, cam.rotation.y, cam.rotation.z);
	printf("Field of view: %2f\n", cam.fov);
	printf("Near plane: %2f\n", cam.nearPlane);
	printf("Far plane: %2f\n", cam.farPlane);
}

void keypress(unsigned char key, int x, int y) {
	switch(key) {

	// Camera controlls
	case 'Q': // Rotate camera counter-clockwise
	case 'q':
		cam.rotation.y -= 0.2f;
		break;
	case 'E': // Rotate camera clockwise
	case 'e':
		cam.rotation.y += 0.2f;
		break;
	case 'W': // Move camera forward
	case 'w':
		cam.position.z += 0.2f;
		break;
	case 'A': // Move camera to the left
	case 'a':
		cam.position.x -= 0.2f;
		break;
	case 'S': // Move camera backwards
	case 's':
		cam.position.z -= 0.2f;
		break;  
	case 'D': // Move camera to the right
	case 'd':
		cam.position.x += 0.2f;
		break;
	case 'Z': // Move camera upwards
	case 'z':
		cam.position.y -= 0.2f;
		break;
	case 'X': // Move camera downwards
	case 'x':
		cam.position.y += 0.2f;
		break;

	// Mesh controlls
	case 'U': // Rotate Mesh counter-clockwise
	case 'u':
		selected->rotation.y -= 0.2f;
		break;
	case 'O': // Rotate Mesh clockwise
	case 'o':
		selected->rotation.y += 0.2f;
		break;
	case 'I': // Move Mesh forward
	case 'i':
		selected->translation.z -= 0.2f;
		break;
	case 'J': // Move Mesh to the left
	case 'j':
		selected->translation.x -= 0.2f;
		break;
	case 'K': // Move Mesh backwards
	case 'k':
		selected->translation.z += 0.2f;
		break;
	case 'L': // Move Mesh to the right
	case 'l':
		selected->translation.x += 0.2f;
		break;
	case '\t': // Toggle between meshes in meshList
		if (selected->next == NULL) {
			selected = meshList;
		}
		else {
			selected = selected->next;
		}
		break;

	// Misc options
	case 'C': // Print camera settings in console
	case 'c':
		CameraSettings();
		break;
	case '0': // Toggle between orthographic- and perpective projection and frustum projection
		viewMode = (viewMode + 1) % 3;
		break;
	case '8': // Toggle between bounce and static
		bounceMode = (bounceMode + 1) % 2;
		break;
	case '9': // Disable faces
		shaderMode = 1;
		break;
	case '�': // Quit
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay(); //S�ger att vi m�ste rita om f�nstret
}

void init(void) {
	// Compile and link the given shader program (vertex shader and fragment shader)
	prepareShaderProgram(vs_n2c_src, fs_ci_src); 

	//Vi m�ste anv�nda depth_test f�r att hantera "h�l" i kossan.
	glEnable(GL_DEPTH_TEST);


	// Setup OpenGL buffers for rendering of the meshes
	Mesh * mesh = meshList;
	while (mesh != NULL) {
		prepareMesh(mesh);
		mesh = mesh->next;
	}	

	selected = meshList;
}

void cleanUp(void) {	
	printf("Running cleanUp function... ");
	// Free openGL resources
	// ...

	// Free meshes
	// ...
	printf("Done!\n\n");
}

// Include data for some triangle meshes (hard coded in struct variables)
#include "./models/mesh_bunny.h"
#include "./models/mesh_cow.h"
#include "./models/mesh_cube.h"
#include "./models/mesh_frog.h"
#include "./models/mesh_knot.h"
#include "./models/mesh_sphere.h"
#include "./models/mesh_teapot.h"
#include "./models/mesh_triceratops.h"


int main(int argc, char **argv) {

	// Setup freeGLUT	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow("DVA338 Programming Assignments");
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keypress);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Specify your preferred OpenGL version and profile
	glutInitContextVersion(4, 5);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);	
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// Uses GLEW as OpenGL Loading Library to get access to modern core features as well as extensions
	GLenum err = glewInit(); 
	if (GLEW_OK != err) { fprintf(stdout, "Error: %s\n", glewGetErrorString(err)); return 1; }

	// Output OpenGL version info
	fprintf(stdout, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, "OpenGL version: %s\n", (const char *)glGetString(GL_VERSION));
	fprintf(stdout, "OpenGL vendor: %s\n\n", glGetString(GL_VENDOR));


	// Insert the 3D models you want in your scene here in a linked list of meshes
	// Note that "meshList" is a pointer to the first mesh and new meshes are added to the front of the list	
	insertModel(&meshList, cow.nov, cow.verts, cow.nof, cow.faces, 20.0);
	Mesh Goat; 
	//LoadObj(&meshList, "./models/Goat.OBJ", &Goat);
	//insertModel(&meshList, triceratops.nov, triceratops.verts, triceratops.nof, triceratops.faces, 3.0);
	//insertModel(&meshList, bunny.nov, bunny.verts, bunny.nof, bunny.faces, 60.0);	
	//insertModel(&meshList, cube.nov, cube.verts, cube.nof, cube.faces, 5.0);
	//insertModel(&meshList, frog.nov, frog.verts, frog.nof, frog.faces, 2.5);
	//insertModel(&meshList, knot.nov, knot.verts, knot.nof, knot.faces, 1.0);
	//insertModel(&meshList, sphere.nov, sphere.verts, sphere.nof, sphere.faces, 12.0);
	//insertModel(&meshList, teapot.nov, teapot.verts, teapot.nof, teapot.faces, 3.0);
	
	
	init();
	glutMainLoop();

	cleanUp();	
	return 0;
}
