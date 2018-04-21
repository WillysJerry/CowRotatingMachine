#include "keys.h"
#include "shader.h"
#include "algebra.h"
#include <stdio.h>

void passiveMouseMotion(int x, int y) {
	static int centerX = player->screen_width / 2;
	static int centerY = player->screen_height / 2;
	Vector rotation;
	float speed = 0.1;

	rotation.x = (y - centerY) * speed;
	rotation.y = (x - centerX) * speed;

	if (rotation.x != 0.0f || rotation.y != 0.0f) {
		player->cam->rotation.x += rotation.x;
		player->cam->rotation.y += rotation.y;
		glutPostRedisplay();

		if (x != centerX || y != centerY)
			glutWarpPointer(centerX, centerY);
	}
}

void keypress(unsigned char key, int x, int y) {
	Camera *cam = player->cam;
	
	switch (key) {
		// Camera controls
	case 'Q': // Rotate camera counter-clockwise
	case 'q':
		cam->rotation.y -= 1.0f;
		break;
	case 'E': // Rotate camera clockwise
	case 'e':
		cam->rotation.y += 1.0f;
		break;
	case 'W': // Move camera forward
		cam->rotation.x += 1.0f;
		break;
	case 'w':
		if (!player->passMouse)
			cam->position.z += 0.2f;
		else
			cam->position = Add(cam->position, ScalarVecMul(0.2f, Normalize(Homogenize(MatVecMul(MatMatMul(RotateY(Deg2Rad(-cam->rotation.y)), RotateX(Deg2Rad(-cam->rotation.x))), { 0, 0, -1 })))));
		break;
	case 'A': // Move camera to the left
		cam->rotation.z -= 1.0f;
		break;
	case 'a':
		if (!player->passMouse)
			cam->position.x -= 0.2f;
		else
			cam->position = Subtract(cam->position, ScalarVecMul(0.2f, CrossProduct(Normalize(Homogenize(MatVecMul(MatMatMul(RotateY(Deg2Rad(-cam->rotation.y)), RotateX(Deg2Rad(-cam->rotation.x))), { 0, 0, -1 }))), Normalize(Homogenize(MatVecMul(RotateZ(Deg2Rad(-cam->rotation.z)), { 0, 1, 0 }))))));
		break;
	case 'S': // Move camera backwards
		cam->rotation.x -= 1.0f;
		break;
	case 's':
		if (!player->passMouse)
			cam->position.z -= 0.2f;
		else
			cam->position = Subtract(cam->position, ScalarVecMul(0.2f, Normalize(Homogenize(MatVecMul(MatMatMul(RotateY(Deg2Rad(-cam->rotation.y)), RotateX(Deg2Rad(-cam->rotation.x))), { 0, 0, -1 })))));
		break;
	case 'D': // Move camera to the right
		cam->rotation.z += 1.0;
		break;
	case 'd':
		if (!player->passMouse)
			cam->position.x += 0.2f;
		else
			cam->position = Add(cam->position, ScalarVecMul(0.2f, CrossProduct(Normalize(Homogenize(MatVecMul(MatMatMul(RotateY(Deg2Rad(-cam->rotation.y)), RotateX(Deg2Rad(-cam->rotation.x))), { 0, 0, -1 }))), Normalize(Homogenize(MatVecMul(RotateZ(Deg2Rad(-cam->rotation.z)), { 0, 1, 0 }))))));
		break;
	case 'Z': // Move camera upwards
	case 'z':
		cam->position.y -= 0.2f;
		break;
	case 'X': // Move camera downwards
	case 'x':
		cam->position.y += 0.2f;
		break;

		// Mesh controlls
	case 'U': // Rotate Mesh counter-clockwise
	case 'u':
		activeMesh->rotation.y -= 1.0f;
		break;
	case 'O': // Rotate Mesh clockwise
	case 'o':
		activeMesh->rotation.y += 1.0f;
		break;
	case 'I': // Move Mesh forward
	case 'i':
		activeMesh->translation.z -= 0.2f;
		break;
	case 'J': // Move Mesh to the left
	case 'j':
		activeMesh->translation.x -= 0.2f;
		break;
	case 'K': // Move Mesh backwards
	case 'k':
		activeMesh->translation.z += 0.2f;
		break;
	case 'L': // Move Mesh to the right
	case 'l':
		activeMesh->translation.x += 0.2f;
		break;
	case 'T': // Move Mesh backwards
	case 't':
		activeMesh->scale.x += 0.2f;
		activeMesh->scale.y += 0.2f;
		activeMesh->scale.z += 0.2f;
		break;
	case 'Y': // Move Mesh backwards
	case 'y':
		activeMesh->scale.x -= 0.2f;
		activeMesh->scale.y -= 0.2f;
		activeMesh->scale.z -= 0.2f;
		break;
	case '\t': // Toggle between meshes in meshList
		if (activeMesh->next == NULL) {
			activeMesh = scene->meshes;
		}
		else {
			activeMesh = activeMesh->next;
		}
		break;

	// Misc options
	case 'V': // Toggle camera modes (free-look and not free-look)
	case 'v':
		player->viewMode = (player->viewMode + 1) % 2;
		break;
	case '0': // Toggle between orthographic- and perpective projection and frustum projection
		player->projMode = (player->projMode + 1) % 3;
		break;
	case '6':
		if (!player->passMouse) {
			player->passMouse = 1;
			glutPassiveMotionFunc(passiveMouseMotion);
		}
		else {
			player->passMouse = 0;
			glutPassiveMotionFunc(NULL);
		}
		break;
	case '7':
		changeShader();
		break;
	//case '8': // Toggle between bounce and static
	//	bounceMode = (bounceMode + 1) % 2;
	//	break;
	case '9': // Disable faces
		player->shaderMode = (player->shaderMode + 1) % 2;
		break;
	case '�': // Quit
		glutLeaveMainLoop();
		break;
	case '+':
		cam->fov += 1;
		break;
	case '-':
		cam->fov -= 1;
		break;
	}
	glutPostRedisplay(); //S�ger att vi m�ste rita om f�nstret
}