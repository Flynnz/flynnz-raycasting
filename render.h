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

typedef struct player
{
	fVector pos, dir, camera;
	fVector speed;
	float rotSpeed;
	SDL_Color color;
}Player;

void RenderBackground(SDL_Renderer* renderer);
void SetRenderColor(SDL_Renderer* renderer, SDL_Color color);
void RenderCell(SDL_Renderer* renderer, int row, int col, SDL_Color border, SDL_Color inner);
void RenderMap(SDL_Renderer* renderer);
void RenderPlayer(SDL_Renderer* renderer, Player* p);
void RenderPlayerBody(SDL_Renderer* renderer, Player* p);
void RenderDirection(SDL_Renderer* renderer, Player* p);
void RenderLaser(SDL_Renderer* renderer, Player* p);
void RenderCamera(SDL_Renderer* renderer, Player* p);
void RotateMatrix(fVector* dir, float angle);
void RenderGridOverlap(SDL_Renderer* renderer, Player* p);
fVector AddVectors(fVector a, fVector b);
fVector ScaleVector(fVector v, float scale);
fVector RotationMatrix(fVector dir, float angle);
fVector perpVectorClockwise(fVector v);
fVector perpVectorCounterClockwise(fVector v);

#endif 
