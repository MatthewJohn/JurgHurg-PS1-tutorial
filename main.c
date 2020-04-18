
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "constants.h"
#include "wall.h"

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   256
#define SCREEN_Z        512
#define VIDEO_TYPE      *(char *)0xBFC7FF52=='E'

typedef struct {
	DISPENV disp;
	DRAWENV draw;
	SVECTOR x[4];
	u_long tag;
	POLY_FT4 shape;
} OT_ELEMENT;

// Guessing at this, as the video did not cover it
typedef struct {
	VECTOR pos;
	SVECTOR ang;
	MATRIX m;
	float pan;
} CAMERA;

static CAMERA cam = {0};

static void init(int x, int y, int z);
static void init_polygon(OT_ELEMENT* polygen, int x1, int y1, int x2, int y2);
static u_short init_tim();
static void set_primatives(CAMERA *cam);
static void update_camera();
static void clearVRAM();
static void display();

int main() {
	OT_ELEMENT polygon[2]; // Init. polygon elements
	OT_ELEMENT* poly_ref;  // Create reference variable
	long p, flag;
	int i, j;

	init((SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2), SCREEN_Z);

	// Initialise both elements and set DRAW and DISP coordinates
	for(i = 0; i <= 1; i++) {
		j = i+1;
		SetDefDrawEnv(&polygon[i].draw, 0, ((1 % j) * SCREEN_HEIGHT), SCREEN_WIDTH, SCREEN_HEIGHT);
		SetDefDispEnv(&polygon[i].disp, 0, ((j % 2) * SCREEN_HEIGHT), SCREEN_WIDTH, SCREEN_HEIGHT);
		init_polygon(&polygon[i], -64, -64, 64, 64);
		polygon[i].shape.tpage = init_tim();
	}
	SetDispMask(1); // Show display

	while (1) {

		// Swap buffer pointer
		if (poly_ref != polygon) poly_ref = polygon;
		else poly_ref++;
		set_primatives(&cam);

		// Reinitialise OT array
		ClearOTag(&poly_ref->tag, 1);

		// Poly's texture coords within the selected TPage
		setUVWH(&poly_ref->shape, 0, 0, 128, 128);

		// Transform perspective
		RotTransPers(&poly_ref->x[0], (long *)&poly_ref->shape.x0, &p, &flag);
		RotTransPers(&poly_ref->x[1], (long *)&poly_ref->shape.x1, &p, &flag);
		RotTransPers(&poly_ref->x[2], (long *)&poly_ref->shape.x2, &p, &flag);
		RotTransPers(&poly_ref->x[3], (long *)&poly_ref->shape.x3, &p, &flag);
		

		// Add polygon to OT
		AddPrim(&poly_ref->tag, &poly_ref->shape);
		display();

		// Switch buffer
		PutDrawEnv(&poly_ref->draw);
		PutDispEnv(&poly_ref->disp);

		// Draw each element
		DrawOTag(&poly_ref->tag);
	}
}

static void init(int x, int y, int z) {
	
	// Initialize hardware and geometry system
	ResetCallback();
	ResetGraph(0);
	InitGeom();
	PadInit(0);
	clearVRAM();
	SetVideoMode(VIDEO_TYPE);

	GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 1);
	SetGeomOffset(x, y);
	SetGeomScreen(z);
}

static void init_polygon(OT_ELEMENT* polygon, int x1, int y1, int x2, int y2) {

	setVector(&polygon->x[0], x1, y1, 0);
	setVector(&polygon->x[1], x2, y1, 0);
	setVector(&polygon->x[2], x1, y2, 0);
	setVector(&polygon->x[3], x2, y2, 0);

	polygon->draw.isbg = 1; // Important (see without!)
	setRGB0(&polygon->draw, 0, 0, 0);
	SetPolyFT4(&polygon->shape);
	SetShadeTex(&polygon->shape, 2);
}

static u_short init_tim() {

	RECT rect;
	GsIMAGE image;

	// Skip image ID and version by incrementing pointer by 0x04
	GsGetTimInfo((u_long *)(wall+4), &image);

	// Load pattern into VRAM
	rect.x = image.px; rect.y = image.py;
	rect.w = image.pw; rect.h = image.ph;
	LoadImage(&rect, image.pixel);

	// Return TPage
	return GetTPage(image.pmode, 1, image.px, image.py);
}

// Sets rotation and translation matricies
static void set_primatives(CAMERA *cam) {

	VECTOR vec;
	update_camera();

	RotMatrix(&cam->ang, &cam->m);
	ApplyMatrixLV(&cam->m, &cam->pos, &vec);
	TransMatrix(&cam->m, &vec);
	SetRotMatrix(&cam->m);
	SetTransMatrix(&cam->m);
}

void update_camera() {

	// Read controller input
	u_short pad_read = PadRead(1);
	if (pad_read) {
		// UP & DOWN
		if(pad_read & PADLup) cam.pos.vz -= 25;
		else if(pad_read & PADLdown) cam.pos.vz += 25;

		// LEFT & RIGHT
		if(pad_read & PADLleft) cam.pan += 2;
		else if(pad_read & PADLright) cam.pan -= 2;

		// L1 & R2
		if(pad_read & PADL1) cam.pos.vx += 5;
		else if(pad_read & PADR1) cam.pos.vx -= 5;

		cam.ang.vy += cam.pan;
	}

	if (cam.pan > -0.02 || cam.pan < 0.02)
		cam.pan *= 0.9; // Reduce panning speed
}

static void clearVRAM() {

	RECT rectTL;
	setRECT(&rectTL, 0, 0, 1024, 512);
	ClearImage2(&rectTL, 0, 0, 0);
	display();
}
