
#include <stdlib.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libmath.h>

#include "constants.h"
#include "controller.h"



float angle;
float px, py;
Line player, pov;

int main() {
	initialize();
	
	while(1) {
		draw();
		update();
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
	px = 155; py = 155;
	player = createLine(createColor(255,255,255), px, py, px, py);
}

void update() {
	padUpdate();

    // Direction buttons
    if (padCheck(Pad1Up)) {
		px = px + (cos(angle)*5);
		py = py + (sin(angle)*5);
	}
    if (padCheck(Pad1Down)) {
		px = px - (cos(angle)*5);
		py = py - (sin(angle)*5);
	}
	
	// Turning buttons
	if (padCheck(Pad1Square)) {
		angle = angle - 0.2;
	}
	if (padCheck(Pad1Circle)) {
		angle = angle + 0.2;
	}
	
	player = moveLine(player, px, py, ((cos(angle)*6) + px), ((sin(angle)*6) + py));
}

void draw() {
	drawLine(pov);
	drawLine(player);
}

