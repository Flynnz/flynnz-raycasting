#ifndef RENDER_H
#define RENDER_H

#define SCREENWIDTH 1080
#define SCREENHEIGHT 980
#define MAPWIDTH 24
#define MAPHEIGHT 24
#include "SDL3/SDL.h"
#include <stdio.h>

typedef enum
{
	noGrid,
	showGrid
}GRID_STATE;

typedef enum
{
	twoD,
	threeD
}RENDER_STATE;

typedef enum
{
	NS,
	EW
}WallSide;

extern SDL_Color SAD_GRAY;
extern SDL_Color LIGHT_GRAY;
extern SDL_Color WEIRD_BLACK;
extern SDL_Color FIRE_ORANGE;
extern SDL_Color COAL_ORANGE;
extern SDL_Color LAPSE_BLUE;
extern SDL_Color HOLLOW_PURPLE;
extern SDL_Color GARDEN_GREEN;

extern float mapToScreenX;
extern float mapToScreenY;
extern int renderState;
extern int gridState;

extern int worldMap[MAPWIDTH][MAPHEIGHT];

typedef struct vector1
{
	int x, y;
}iVector;

typedef struct vector2
{
	float x, y;
}fVector;

extern fVector MAX_len;

typedef struct camera
{
	fVector dir, leftmostRay, rightmostRay;
	float angle;
}Camera;

typedef struct player
{
	fVector pos, dir;
	fVector speed;
	float rotSpeed;
	SDL_Color color;
}Player;

Player InitPlayer(SDL_Color color, iVector startPos, fVector dir, float speed, float rotSpeed);
void UpdatePlayerSpeed(Player* p, float speed);
Camera InitCamera(Player p, float angle);
void RenderBackground(SDL_Renderer* renderer);
void SetRenderColor(SDL_Renderer* renderer, SDL_Color color);
void RenderCell(SDL_Renderer* renderer, int row, int col, fVector startPos, fVector ratio, SDL_Color border, SDL_Color inner);
void RenderMap(SDL_Renderer* renderer, fVector startPos, fVector ratio);
void RenderPlayer(SDL_Renderer* renderer, Player* p, fVector ratio, fVector offset);
void RenderPlayerBody(SDL_Renderer* renderer, Player* p, fVector ratio, fVector offset);
void RenderDirection(SDL_Renderer* renderer, Player* p, fVector ratio, fVector offset);
void RenderLaser(SDL_Renderer* renderer, Player* p, fVector ratio, fVector offset);
void RenderCamera(SDL_Renderer* renderer, Player* p, Camera cam, fVector ratio, fVector offset);
void HighlightCell(SDL_Renderer* renderer, SDL_FRect cell, SDL_Color inside, SDL_Color border);
void RenderGridOverlap(SDL_Renderer* renderer, Player* p, fVector ratio, fVector offset);
float DDA(fVector rayDir, Player p, SDL_Renderer* renderer, int* wallSide, fVector ratio, fVector offset);
float ShootRay(SDL_Renderer* renderer, fVector rayDir, Player p, float* distX, float* distY, 
				int* sideHit, float deltaX, float deltaY, iVector currCell, fVector ratio, fVector offset);
void HandlePlayerMovement(const bool* keystate, Player* p);
void HandleRotation(const bool* keystate, Player* p, Camera* cam);
fVector DetermineRayDir(float angle, fVector leftmostRay);
void CountFPS(SDL_Renderer* renderer, Uint64 fpsLastTime, int fpsFrames, float fps, char fpsText[]);
void SetScaleAndOffset(fVector* mapStartPos, fVector* mapToScreenRatio);

fVector AddVectors(fVector a, fVector b);
fVector ScaleVector(fVector v, float scale);
void RotateMatrix(fVector* dir, float angle);
fVector RotationMatrix(fVector dir, float angle);
fVector perpVectorClockwise(fVector v);
fVector perpVectorCounterClockwise(fVector v);

#endif 
