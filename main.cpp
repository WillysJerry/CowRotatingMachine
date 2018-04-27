//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <glew.h>
#include <freeglut.h>
#include <math.h>
#include "algebra.h"
#include "light.h"
#include "mesh.h"
#include "player.h"
#include "keys.h"
#include "shader.h"
#include "loader.h"

float t = 0; // Temporary time variable for mesh bounce animation

void changeSize(int w, int h) {
	player->screen_width = w;
	player->screen_height = h;
	glViewport(0, 0, player->screen_width, player->screen_height);
}

void init(void) {
	const char * shprgs[][2] = {{ "shaders/default_vertex.glsl", "shaders/default_fragment.glsl" }, //Default
								{ "shaders/default_vertex.glsl", "shaders/cartoon_fragment.glsl" }, //Cartoon
								{ "shaders/cartoonphong_vertex.glsl", "shaders/cartoonphong_fragment.glsl" },	// Lit cartoon 420 blaze it
								{ "shaders/light_vertex.glsl", "shaders/light_fragment.glsl" },		//Light
								{ "shaders/gourad_vertex.glsl", "shaders/gourad_fragment.glsl"}		//Gourad
							   };
	static const char * vs[1];
	static const char * fs[1];
	int i = 0, slen = sizeof(shprgs) / sizeof(shprgs[0]);

	Shader *head = NULL;
	player->shader = (Shader*)malloc(sizeof(Shader));
	do {
		player->shader->shaderFiles[0] = shprgs[i][0];
		player->shader->shaderFiles[1] = shprgs[i][1];
		
		if (i == 0) {
			readShaderFile(shprgs[i][0], vs);
			readShaderFile(shprgs[i][1], fs);
			head = player->shader;
			player->shader->program = prepareShaderProgram(vs, fs);
		}
		if (i == slen - 1)
			player->shader->next = head;
		else
			player->shader->next = (Shader*)malloc(sizeof(Shader));
		player->shader = player->shader->next;
		i++;
	} while (i < slen);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Setup OpenGL buffers for rendering of the meshes
	Mesh *mesh = scene->meshes;
	while (mesh != NULL) {
		prepareMesh(mesh, player->shader->program);
		mesh = mesh->next;
	}	

	activeMesh = scene->meshes;

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
//#include "./models/mesh_cube.h"
#include "./models/mesh_frog.h"
//#include "./models/mesh_knot.h"
#include "./models/mesh_sphere.h"
//#include "./models/mesh_teapot.h"
#include "./models/mesh_triceratops.h"

//Please, for the love of God, don't use this!
void idleFunc() {
	if (player->passMouse) {
		if (   player->cameraMovement.x == 1
			|| player->cameraMovement.y == 1
			|| player->cameraMovement.z == 1
			|| player->cameraMovement.w == 1)
			glutPostRedisplay();
	}
}

int main(int argc, char **argv) {

	static Camera camera =
	{
		{ 0, 0, 20 },	//Position
		{ 0, 0, 0 },	//Rotation
		30,				//Fov
		10,				//Near plane
		10000			//Far plane
	};

	player->cam = &camera;
	player->screen_width = 1024;
	player->screen_height = 768;
	player->shader = NULL;

	// Setup freeGLUT	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(player->screen_width, player->screen_height);
	glutCreateWindow("DVA338 Programming Assignments");
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(keyUp);
	glutPassiveMotionFunc(NULL);
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
	//GLint texture = loadBMP("models/WoodenCabin/WoodCabinDif.bmp");

	//Material matRed = {
	//	{ 0.1f, 0.1f, 0.1f },	// Ambient
	//	{ 0.7f, 0.1f, 0.1f },	// Diffuse
	//	{ 1.0f, 1.0f, 1.0f },	// Specular
	//	texture,
	//	20.0f					// Shininess
	//};

	//Material matBlue = {
	//	{ 0.1f, 0.1f, 0.1f },	// Ambient
	//	{ 0.1f, 0.1f, 0.7f },	// Diffuse
	//	{ 1.0f, 1.0f, 1.0f },	// Specular
	//	texture,
	//	20.8f					// Shininess
	//};

	//Material matWhite = {
	//	{ 0.1f, 0.1f, 0.1f },	// Ambient
	//	{ 1.0f, 1.0f, 1.0f },	// Diffuse
	//	{ 1.0f, 1.0f, 1.0f },	// Specular
	//	texture,
	//	1.0f					// Shininess
	//};



	/*
	insertModel(&scene->meshes, cow.nov, cow.verts, cow.nof, cow.faces, matRed, 20.0);
	scene->meshes->rotation = { 0, 0, 80 };
	scene->meshes->translation = { -1, 0, 2 };

	//insertModelFromFile(&meshList, "./models/Goat.OBJ");
	//insertModelFromFile(&scene->meshes, "./models/m16.OBJ");
	//scene->meshes->scale = { 1.25f, 1.25f, 1.25f };
	//scene->meshes->translation = { 7, 2, 1.75f };

	insertModel(&scene->meshes, triceratops.nov, triceratops.verts, triceratops.nof, triceratops.faces, matRed, 3.0);
	scene->meshes->translation = { -20, 0, 20 };
	scene->meshes->rotation = { 0, 25, 0 };


	insertModel(&scene->meshes, bunny.nov, bunny.verts, bunny.nof, bunny.faces, matBlue, 60.0);
	scene->meshes->translation = { 0, 0, 40 };
	scene->meshes->rotation = { 0, 180, 0 };

	//insertModel(&meshList, cube.nov, cube.verts, cube.nof, cube.faces, 5.0);
	insertModel(&scene->meshes, frog.nov, frog.verts, frog.nof, frog.faces, matBlue, 1.0);
	scene->meshes->translation = { 20, 0, 20 };
	scene->meshes->rotation = { 0, -60, 0 };

	//insertModel(&meshList, knot.nov, knot.verts, knot.nof, knot.faces, 0.5);
	*/
	//PointLight pl =
	//{
	//	{ 5, 10, 3 },
	//	{0.1f, 0.02f, 0.02f},
	//	{0.7f, 0.7f, 0.7f},
	//	{1, 1, 1},
	//	0.002f,
	//	NULL
	//};

	//PointLight pl2 =
	//{
	//	{ -8, 7, 3 },
	//	{ 0.02f, 0.1f, 0.02f},
	//	{0.7f, 0.7f, 0.7f},
	//	{1, 1, 1},
	//	0.002f,
	//	NULL
	//};

	//pl.next = &pl2;

	//scene->pointLights = &pl;


	/*
	insertModel(&scene->meshes, sphere.nov, sphere.verts, sphere.nof, sphere.faces, matWhite, 1.0);
	scene->meshes->translation = scene->pointLights->pos;
	scene->meshes->scale = { -1, -1, -1 };
	
	insertModel(&scene->meshes, sphere.nov, sphere.verts, sphere.nof, sphere.faces, matWhite, 1.0);
	scene->meshes->translation = scene->pointLights->next->pos;
	scene->meshes->scale = { -1, -1, -1 };
	*/

	//insertModel(&meshList, teapot.nov, teapot.verts, teapot.nof, teapot.faces, 1.0);
	
	GLint texCabin = loadBMP("models/WoodenCabin/WoodCabinDif.bmp");

	Material matCabin = {
		{ 0.1f, 0.1f, 0.1f },	// Ambient
		{ 0.7f, 0.7f, 0.7f },	// Diffuse
		{ 1.0f, 1.0f, 1.0f },	// Specular
		texCabin,
		10.0f					// Shininess
	};

	Mesh* meshCabin = loadObj("models/WoodenCabin/WoodenCabinObj.obj", 8439, 8439, 8439, 2813);
	insertMesh(&scene->meshes, meshCabin, matCabin, 0.5);
	scene->meshes->translation = { 0, -15, 5 };


	GLint texMarty = loadBMP("models/Marty/Marty.bmp");

	Material matMarty = {
		{ 0.1f, 0.1f, 0.1f },	// Ambient
		{ 0.7f, 0.7f, 0.7f },	// Diffuse
		{ 1.0f, 1.0f, 1.0f },	// Specular
		texMarty,
		10.0f					// Shininess
	};

	Mesh* meshMarty = loadObj("models/Marty.obj", 5793, 5793, 5793, 1931);
	insertMesh(&scene->meshes, meshMarty, matMarty, 3.0);
	scene->meshes->translation = { 4, -10.35f, -7 };
	scene->meshes->rotation = { 0, -43, 0 };



	addPointLight(&scene->pointLights, { 0, 2, -5 });
	addPointLight(&scene->pointLights, { -5, 20, 35 }, { 0.2f, 0.2f, 0.2f }, { 1.4f, 1.4f, 1.4f }, { 2, 2, 2 }, 0.002f);


	//Mesh* goat = loadObj("models/WoodenCabin/WoodenCabinObj.obj", 71595, 71595, 71595, 23865); //If no values filled in, default values are used loadObj("file") is a valid call
	//insertMesh(&scene->meshes, goat, matWhite, 0.5);
	//scene->meshes->translation = { 0, 0, 0 };


	init();
	glutMainLoop();

	cleanUp();	
	return 0;
}
