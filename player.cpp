#include "algebra.h"
#include "player.h"

Player *player = (Player*)calloc(1, sizeof(Player));

void display(void) {
	static Matrix V, P, PV;
	Mesh *mesh;
	Camera *cam = player->cam;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


	float a = Deg2Rad(cam->rotation.x), b = Deg2Rad(cam->rotation.y), c = Deg2Rad(cam->rotation.z);
	Vector pos = { -cam->position.x, -cam->position.y, -cam->position.z };

	Matrix rz = RotateZ(-c);
	Matrix ry = RotateY(-b);
	Matrix rx = RotateX(-a);
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
	glUseProgram(player->shprg);

	// Render all meshes in the scene
	mesh = scene->meshes;

	while (mesh != NULL) {
		renderMesh(mesh, V, P, PV);
		mesh = mesh->next;
	}

	glFlush();
}
