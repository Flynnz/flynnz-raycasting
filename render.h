#ifndef RENDER_H
#define RENDER_H

#define SCREENWIDTH 900
#define SCREENHEIGHT 900
#define MAPWIDTH 24
#define MAPHEIGHT 24
#include "SDL3/SDL.h"

typedef enum
{
	noGrid,
	showGrid
}GRID_STATE;

extern SDL_Color SAD_GRAY;
extern SDL_Color LIGHT_GRAY;
extern SDL_Color COOL_ORANGE;
extern SDL_Color WEIRD_BLACK;
extern SDL_Color FIRE_RED;
extern SDL_Color FREEZE_BLUE;
extern SDL_Color HOLLOW_PURPLE;
extern SDL_Color ROT_GREEN;


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

typedef struct player
{
	fVector pos, dir, camera;
	fVector speed;
	float rotSpeed;
	SDL_Color color;
}Player;

Player InitPlayer(SDL_Color color, iVector startPos, fVector dir, float speed, float rotSpeed);
void RenderBackground(SDL_Renderer* renderer);
void SetRenderColor(SDL_Renderer* renderer, SDL_Color color);
void RenderCell(SDL_Renderer* renderer, int row, int col, SDL_Color border, SDL_Color inner);
void RenderMap(SDL_Renderer* renderer);
void RenderPlayer(SDL_Renderer* renderer, Player* p);
void RenderPlayerBody(SDL_Renderer* renderer, Player* p);
void RenderDirection(SDL_Renderer* renderer, Player* p);
void RenderLaser(SDL_Renderer* renderer, Player* p);
void RenderCamera(SDL_Renderer* renderer, Player* p, fVector leftmostRay, fVector rightmostRay);
void HighlightCell(SDL_Renderer* renderer, SDL_FRect cell, SDL_Color inside, SDL_Color border);
void RenderGridOverlap(SDL_Renderer* renderer, Player* p);
void DDA(fVector rayDir, Player p, SDL_Renderer* renderer);
void HandlePlayerMovement(bool* keystate, Player* p);
void HandleRotation(bool* keystate, Player* p);
fVector DetermineRayDir(float angle, fVector leftmostRay);
fVector AddVectors(fVector a, fVector b);
fVector ScaleVector(fVector v, float scale);
void RotateMatrix(fVector* dir, float angle);
fVector RotationMatrix(fVector dir, float angle);
fVector perpVectorClockwise(fVector v);
fVector perpVectorCounterClockwise(fVector v);
float Norma(fVector vect);
float radians_to_degrees(float radians);

#endif 
