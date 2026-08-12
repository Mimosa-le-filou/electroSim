#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include <math.h>
#include <stdio.h>
// To run the file :
// gcc electro.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 && ./a.out
// A few other stuff
// Add van der waals forces and velocity verlet algorithm, with temperature coupling and you will find urself a full Molecular Dynamics toy model
//---------------------------------------------------------------------------------
// Defines and Macro

#define SCREENWIDTH 1000
#define SCREENHEIGHT 650
#define CHARGES_NUMBER 200

const float CUBE_D = 30;
const float RADIUS = 0.2;
const float SPEED = 10;
//---------------------------------------------------------------------------------
//
// Typedefs 
typedef struct {
	int sign;
	float x, y, z;
	float vx, vy, vz;
	float fx, fy, fz;
	float r;
} Charge;

Charge charges[CHARGES_NUMBER];



// Function definitions
void InitCharges(){
	for(int i = 0; i< CHARGES_NUMBER; i++){
		charges[i].sign = (i < CHARGES_NUMBER/2) ? -1 : 1;
		charges[i].r = RADIUS;
		charges[i].vx = GetRandomValue(-SPEED , SPEED );
		charges[i].vy = GetRandomValue(-SPEED , SPEED );
		charges[i].vz = GetRandomValue(-SPEED , SPEED );
		charges[i].fx = 0;
		charges[i].fy = 0;
		charges[i].fz = 0;
		charges[i].x = GetRandomValue(-CUBE_D/2 + RADIUS, CUBE_D/2 - RADIUS);
		charges[i].y = GetRandomValue(-CUBE_D/2 + RADIUS, CUBE_D/2 - RADIUS);
		charges[i].z = GetRandomValue(-CUBE_D/2 + RADIUS, CUBE_D/2 - RADIUS);
	}
}

void DrawCharges(){
	Color color;
	for(int i = 0; i< CHARGES_NUMBER; i++){
		switch(charges[i].sign){
			case 1:
				color = RED;
				break;
			case -1:
				color = BLUE;
				break;
			default:
				printf("Error : Invalid charge sign\n");
		}
		DrawSphere((Vector3){charges[i].x, charges[i].y, charges[i].z}, charges[i].r, color);
	}
}


void ProcessForces(){
	Charge *c1;
	Charge *c2;
	for(int i = 0; i < CHARGES_NUMBER; i++){
		c1 = &charges[i];
		for(int j = i + 1; j < CHARGES_NUMBER; j++){
			c2 = &charges[j];
			// Distance
			float dx = c2->x - c1->x;
			float dy = c2->y - c1->y;
			float dz = c2->z - c1->z;
			float d2 = dx*dx + dy*dy + dz*dz;

			if(d2 < 1){d2 = 1;}
			float d = sqrtf(d2);
			float nx = dx/d;	
			float ny = dy/d;	
			float nz = dz/d;	

			float force = 5000*c1->sign * c2->sign / d2;
			float fx = force * nx;
			float fy = force * ny;
			float fz = force * nz;	
			c1->fx -= fx;
			c1->fy -= fy;
			c1->fz -= fz;
			c2->fx = fx;
			c2->fy = fy;
			c2->fz = fz;
		}
	}
}

void UpdateCharges(float dt){
	Charge *c;
	for(int i = 0; i < CHARGES_NUMBER; i++){
		c = &charges[i];
		c->vx += c->fx * dt;
		c->vy += c->fy * dt;
		c->vz += c->fz * dt;
		c->x += c->vx * dt;
		c->y += c->vy * dt;
		c->z += c->vz * dt;

		c->vx *= .99f;
		c->vy *= .99f;
		c->vz *= .99f;
	}
}

void ResetForces(){
	Charge *c;
	for(int i = 0; i < CHARGES_NUMBER; i++){
		c = &charges[i];
		c->fx = 0;
		c->fy = 0;
		c->fz = 0;
	}
}

void CollideWalls(){
	Charge *c;
	for(int i = 0; i < CHARGES_NUMBER; i++){
		c = &charges[i];
		if (c->x < -CUBE_D/2 + c->r){
			c-> vx *= -1;
			c->x = -CUBE_D/2 + c->r;
		}
		else if (c->x > CUBE_D/2 - c->r){
			c->vx *= -1;
			c->x = CUBE_D/2 - c->r;
		}
		if (c->y < -CUBE_D/2 + c->r){
			c->vy *=-1;
			c->y = -CUBE_D/2 + c ->r;	
		}
		else if (c->y > CUBE_D/2 - c->r){
			c->vy *= -1;	
			c->y = CUBE_D/2 - c->r;
		}
		if (c->z < -CUBE_D/2 + c->r){
			c->vz *= -1;
			c->z = -CUBE_D/2 + c->r;
		}
		else if(c->z > CUBE_D/2 - c->r){
			c->vz *= -1;
			c->z = CUBE_D/2 - c->r;	
		}
	}
}


void Chaos(){
	Charge* c;
	for (int i = 0; i < CHARGES_NUMBER; i++){
		c = &charges[i];	
		c->vx += GetRandomValue(-SPEED*10 , SPEED*10 );
		c->vy += GetRandomValue(-SPEED*10, SPEED*10 );
		c->vz += GetRandomValue(-SPEED*10 , SPEED*10 );

	}
}



int main(void){
	// Initialization
	InitWindow(SCREENWIDTH, SCREENHEIGHT, "3D electrostatic simulation");
	Camera3D camera = { 0 };
	camera.position = (Vector3) { 20.0f, 20.0f, 20.0f}; // Camera position
	camera.target = (Vector3) {0.0f, 0.0f, 0.0f};	// Camera looking  at point
	camera.up = (Vector3){0.0f, 1.0f, 0.0f};	// Camera up vector (rotation towards target)
	camera.fovy = 90.0f;	// Camera field-of-view
	camera.projection = CAMERA_PERSPECTIVE;
	Vector3 cubePosition = {0.0f, 0.0f, 0.0f};

	SetTargetFPS(120);
	InitCharges();


	while(!WindowShouldClose())
	{
		//Zoom
		CameraMoveToTarget(&camera, -GetMouseWheelMove());

		// Rotating the cube using mouse left click
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
			Vector2 mouseDelta = GetMouseDelta();
			Matrix rotationX = MatrixRotate(camera.up, -mouseDelta.x/100);
			Matrix rotationY = MatrixRotate(Vector3CrossProduct(camera.position, camera.up), mouseDelta.y/100);

			Vector3 view = Vector3Subtract(camera.position, camera.target);
			view = Vector3Transform(view, rotationX);
			view = Vector3Transform(view, rotationY);
			
			camera.position = Vector3Add(camera.target, view);	
		}


		// Creating movement
		if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)){
			Chaos();
		}


		// Draw
		BeginDrawing();
			ClearBackground(BLACK);
			BeginMode3D(camera);
				DrawCubeWires(cubePosition, CUBE_D,CUBE_D, CUBE_D, WHITE);
				ResetForces();
				ProcessForces();
				UpdateCharges(GetFrameTime()/5);	
				CollideWalls();
				DrawCharges();
			EndMode3D();

		EndDrawing();
	}
	CloseWindow();
	return 0;	// Close window and OpenGL context
}
