#include <stdio.h>
#include "algebra.h"
#include "player.h"

Player *player = (Player*)calloc(1, sizeof(Player));

Vector Deg2RadVec(Vector input) {
	return { Deg2Rad(input.x), Deg2Rad(input.y), Deg2Rad(input.z) };
}

void moveCamera(int moved) {
	static Camera *cam = player->cam;
	static Matrix rz, ry, rx;
	static Vector gaze, up, rotation;

	if (player->cameraMovement.x == 1 || player->cameraMovement.y == 1 || player->cameraMovement.z == 1 || player->cameraMovement.w == 1) {
		rotation = Deg2RadVec(cam->rotation);
		rz = RotateZ(-rotation.z);
		ry = RotateY(-rotation.y);
		rx = RotateX(-rotation.x);
		gaze = Normalize(Homogenize(MatVecMul(MatMatMul(ry, rx), { 0, 0, -1 })));
		up = Normalize(Homogenize(MatVecMul(rz, { 0, 1, 0 })));
	}

	if (!moved)
		glutTimerFunc(1000 / 30, moveCamera, 0);

	if (player->cameraMovement.x + player->cameraMovement.y == 1) { //Move in x-axis
		if (player->cameraMovement.x == 1) {
			cam->position = Subtract(cam->position, ScalarVecMul(0.1f, CrossProduct(gaze, up)));
		}
		else if (player->cameraMovement.y == 1) {
			cam->position = Add(cam->position, ScalarVecMul(0.1f, CrossProduct(gaze, up)));
		}
		moved = 1;
	}

	if (player->cameraMovement.z + player->cameraMovement.w == 1) { // Move in z-axis
		if (player->cameraMovement.z == 1) {
			cam->position = Add(cam->position, ScalarVecMul(0.1f, gaze));
		}
		else if (player->cameraMovement.w == 1) {
			cam->position = Subtract(cam->position, ScalarVecMul(0.1f, gaze));
		}
		moved = 1;
	}

	if (moved)
		glutPostRedisplay();
}

void display(void) {
	static Matrix V, P, PV;
	Mesh *mesh;
	Camera *cam = player->cam;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


	//float a = Deg2Rad(cam->rotation.x), b = Deg2Rad(cam->rotation.y), c = Deg2Rad(cam->rotation.z);
	Vector rotation = Deg2RadVec(cam->rotation);
	Vector pos = { -cam->position.x, -cam->position.y, -cam->position.z };

	Matrix rz = RotateZ(-rotation.z);
	Matrix ry = RotateY(-rotation.y);
	Matrix rx = RotateX(-rotation.x);
	Matrix t = Translate(pos.x, pos.y, pos.z);


	if (player->viewMode == 0) {
		V = MatMatMul(t, MatMatMul(rx, MatMatMul(rz, ry)));
	}
	else {
		// Free look
		Matrix a = MatMatMul(ry, rx);	// Pls note we only rotate around the x- and y-axes when we determine the gaze direction

		Vector eye = cam->position;											// the cam position
		Vector gaze = Homogenize(MatVecMul(a, { 0, 0, -1 }));				// Gaze is any direction the viewer looks at
																			//Vector up = { 0, 1, 0 };											// Screen up is always up
		Vector up = Normalize(Homogenize(MatVecMul(rz, { 0, 1, 0 })));	// Direction pointing up from the viewer

		V = MatLookAt(eye, gaze, up);		// Then we do this <- over there on the left
	}

	// Assignment 1: Calculate the projection transform yourself 	
	// The matrix P should be calculated from camera parameters
	// Therefore, you need to replace this hard-coded transform. 	

	if (player->projMode == 0) {
		P = MatOrtho(-10, 10, -10, 10, 0, 100000);
	}
	else if (player->projMode == 1) {
		P = MatPerspective(Deg2Rad(cam->fov), player->screen_width / player->screen_height, 1, 100000);
	}
	else {
		P = MatFrustum(-1, 1, -1, 1, 1, 100000);

	}

	// This finds the combined view-projection matrix
	PV = MatMatMul(P, V);

	// Select the shader program to be used during rendering 
	glUseProgram(player->shader->program);

	// Render all meshes in the scene
	mesh = scene->meshes;

	while (mesh != NULL) {
		renderMesh(mesh, V, P, PV);
		mesh = mesh->next;
	}

	glFlush();
}
