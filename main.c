
#include <stdlib.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libmath.h>

#include "constants.h"
#include "controller.h"

const int START_X = 160, START_Y = 128;

float angle, c_ang, s_ang;
float px, py;
float lx1, ly1, lx2, ly2;
float tx1, ty1, tx2, ty2, tz1, tz2;
Line player;
Line line;

void initialize();
void update();
void display();
void moveSurroundings();

int main() {
	initialize();
	
	while(1) {
		draw();
		update();
		moveSurroundings();
		display();
	}
	
	return 0;
}

void initialize() {
	initializeScreen();
	initializePad();
	initializeDebugFont();
	setBackgroundColor(createColor(0,0,0));
	
	// Set intial coordinates of the player
	angle = 0;
	px = START_X;
	py = START_Y;
	
	lx1 = START_X + 20;
	ly1 = START_Y - 30;
	lx2 = START_X + 20;
	ly2 = START_Y + 30;
	
	player = createLine(createColor(255,255,255), px, py, px, py-5);
	line = createLine(createColor(255,255,255), lx1, ly1, lx2, ly2);
}

void update() {
	padUpdate();

    // Direction buttons
    if (padCheck(Pad1Up)) {
		px = px + (c_ang*5);
		py = py + (s_ang*5);
	}
    if (padCheck(Pad1Down)) {
		px = px - (c_ang*5);
		py = py - (s_ang*5);
	}
	
	// Turning buttons
	if (padCheck(Pad1Square)) {
		angle = angle - 0.2;
	}
	if (padCheck(Pad1Circle)) {
		angle = angle + 0.2;
	}
}

void moveSurroundings() {
	c_ang = cos(angle);
	s_ang = sin(angle);
	
	// Transform vertexes relative to the player
	tx1 = lx1 - px;
	ty1 = ly1 - py;
	tx2 = lx2 - px;
	ty2 = ly2 - py;
	
	// Rotate them around the player
	tz1 = (tx1 * c_ang) + (ty1 * s_ang);
	tz2 = (tx2 * c_ang) + (ty2 * s_ang);
	tx1 = (tx1 * s_ang) - (ty1 * c_ang);
	tx2 = (tx2 * s_ang) - (ty2 * c_ang);
	
	line = moveLine(line, (START_X - tx1), (START_Y - tz1), (START_X - tx2), (START_Y - tz2));
	
}

void draw() {
	drawLine(player);
	drawLine(line);
}

