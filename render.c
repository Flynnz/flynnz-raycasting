#include <stdio.h>

#include "render.h"
#define PI 3.14159265

SDL_Color SAD_GRAY = { 85, 85, 90, 255 };
SDL_Color GHOST_GRAY = { 110, 110, 110, 55 };
SDL_Color LIGHT_GRAY = { 95, 95, 95, 255 };
SDL_Color WEIRD_BLACK = { 20, 30, 20, 255 };
SDL_Color FIRE_ORANGE = { 150, 50, 20, 255 };
SDL_Color COAL_ORANGE = { 180, 50, 30, 255 };
SDL_Color FREEZE_BLUE = { 20, 50, 150, 255 };
SDL_Color HOLLOW_PURPLE = { 150, 20, 150, 205 };
SDL_Color ROT_GREEN = { 15, 120, 20, 215 };

fVector MAX_len = { (float)SCREENWIDTH * 100, (float)SCREENHEIGHT * 100 };
int gridState = showGrid;
int renderState = twoD;
float mapToScreenX = (float)SCREENWIDTH / MAPWIDTH;
float mapToScreenY = (float)SCREENHEIGHT/ MAPHEIGHT;

Player InitPlayer(SDL_Color color, iVector startPos, fVector dir, float speed, float rotSpeed)
{
	Player p;
	p.color = color;
	p.pos.x = (float)startPos.x;
	p.pos.y = (float)startPos.y;

	p.dir.x = dir.x;
	p.dir.y = dir.y;

	p.speed.x = speed;
	p.speed.y = speed;
	p.rotSpeed = rotSpeed;

	return p;
}

Camera InitCamera(Player p, float angle)
{
	Camera result;
	result.leftmostRay = RotationMatrix(p.dir, angle / 2.0f);
	result.rightmostRay = RotationMatrix(p.dir, -angle / 2.0f);
	result.angle = angle;
	result.dir = perpVectorClockwise(p.dir);

	return result;
}

void RenderBackground(SDL_Renderer* renderer)
{
	SetRenderColor(renderer, WEIRD_BLACK);
	SDL_RenderClear(renderer);
}

void RenderMap(SDL_Renderer* renderer, fVector startPos, fVector ratio)
{
	int row, col;
	for (row = 0; row < MAPHEIGHT; row++)
	{
		for (col = 0; col < MAPWIDTH; col++)
		{
			if (worldMap[row][col] > 0)
				RenderCell(renderer, row, col, startPos, ratio, WEIRD_BLACK, FIRE_ORANGE);
			else
				RenderCell(renderer, row, col, startPos, ratio, SAD_GRAY, LIGHT_GRAY);
		}
	}
}

void SetRenderColor(SDL_Renderer* renderer, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void RenderCell(SDL_Renderer* renderer, int row, int col, fVector startPos, fVector ratio, SDL_Color border, SDL_Color inner)
{
	SDL_FRect cell;
	cell.w = ratio.x;
	cell.h = ratio.y;
	cell.x = (float)(col * ratio.x + startPos.x);
	cell.y = (float)(row * ratio.y + startPos.y);

	SetRenderColor(renderer, inner);
	SDL_RenderFillRect(renderer, &cell);
	
	if (gridState == showGrid)
	{
		SetRenderColor(renderer, border);
		SDL_RenderRect(renderer, &cell);
	}
}

void RenderPlayer(SDL_Renderer* renderer, Player* p, fVector ratio, fVector offset)
{
	RenderDirection(renderer, p, ratio, offset);
	RenderPlayerBody(renderer, p, ratio, offset);
}

void RenderPlayerBody(SDL_Renderer* renderer, Player* p, fVector ratio, fVector offset)
{
	SDL_FRect hitbox;
	hitbox.w = ratio.x / 3;
	hitbox.h = ratio.y / 3;

	//tricks the renderer to start from the center
	hitbox.x = (float)((p->pos.x) * ratio.x - hitbox.w / 2 + offset.x);
	hitbox.y = (float)((p->pos.y) * ratio.y - hitbox.h / 2 + offset.y);

	SetRenderColor(renderer, p->color);
	SDL_RenderFillRect(renderer, &hitbox);
}

void RenderDirection(SDL_Renderer* renderer, Player* p, fVector ratio, fVector offset)
{
	fVector start, end;

	start.x = p->pos.x * ratio.x + offset.x;
	start.y = p->pos.y * ratio.y + offset.y;
	end.x = start.x + p->dir.x * ratio.x;
	end.y = start.y + p->dir.y * ratio.y;

	float tipLenght;
	tipLenght = (float)1 / 4 * ratio.x;
	fVector tipDir = RotationMatrix(p->dir, (float)PI/4);
	fVector tipVector = ScaleVector(tipDir, -tipLenght);
	fVector tipEnd1 = AddVectors(end, tipVector);
	fVector tipEnd2 = AddVectors(end, perpVectorCounterClockwise(tipVector));

	SetRenderColor(renderer, FREEZE_BLUE);

	//arrow body
	SDL_RenderLine(renderer, start.x, start.y, end.x, end.y);
	SDL_RenderLine(renderer, start.x + p->dir.y, start.y + p->dir.x, end.x + p->dir.y, end.y + p->dir.x);
	SDL_RenderLine(renderer, start.x - p->dir.y, start.y - p->dir.x, end.x - p->dir.y, end.y - p->dir.x);

	//arrow tip
	SDL_RenderLine(renderer, end.x, end.y, tipEnd1.x, tipEnd1.y);
	SDL_RenderLine(renderer, end.x, end.y, tipEnd2.x, tipEnd2.y);
}

void RenderLaser(SDL_Renderer* renderer, Player* p, fVector ratio, fVector offset)
{
	fVector start, end;
	start.x = p->pos.x * ratio.x + offset.x;
	start.y = p->pos.y * ratio.y + offset.y;
	end.x = start.x + p->dir.x * MAX_len.x;
	end.y = start.y + p->dir.y * MAX_len.x;

	SetRenderColor(renderer, FIRE_ORANGE);
	SDL_RenderLine(renderer, start.x, start.y, end.x, end.y);
}

void RenderCamera(SDL_Renderer* renderer, Player* p, Camera cam, fVector ratio, fVector offset)
{
	//limiting rays
	fVector endLeft, endRight;
	fVector start = { p->pos.x * ratio.x + offset.x, p->pos.y * ratio.y + offset.y };
	
	endLeft = ScaleVector(cam.leftmostRay, MAX_len.x);
	endRight = ScaleVector(cam.rightmostRay, MAX_len.x);

	SetRenderColor(renderer, HOLLOW_PURPLE);
	SDL_RenderLine(renderer, start.x, start.y, endLeft.x, endLeft.y);
	SDL_RenderLine(renderer, start.x, start.y, endRight.x, endRight.y);
	
	//camera plane
	fVector dirEnd = AddVectors(p->dir, p->pos);

	dirEnd.x *= ratio.x;
	dirEnd.y *= ratio.y;
	fVector cameraEnd1 = AddVectors(dirEnd, ScaleVector(cam.dir, MAX_len.x));
	fVector cameraEnd2 = AddVectors(dirEnd, ScaleVector(cam.dir, -MAX_len.x));

	SetRenderColor(renderer, ROT_GREEN);
	SDL_RenderLine(renderer, dirEnd.x + offset.x, dirEnd.y + offset.y, cameraEnd1.x, cameraEnd1.y);
	SDL_RenderLine(renderer, dirEnd.x + offset.x, dirEnd.y + offset.y, cameraEnd2.x, cameraEnd2.y);
}

void HighlightCell(SDL_Renderer* renderer, SDL_FRect cell, SDL_Color inside, SDL_Color border)
{
	SetRenderColor(renderer, inside);
	SDL_RenderFillRect(renderer, &cell);
	SetRenderColor(renderer, border);
	SDL_RenderRect(renderer, &cell);
}

void RenderGridOverlap(SDL_Renderer *renderer, Player *p, fVector ratio, fVector offset)
{
	//player
	SDL_FRect playerCell;
	playerCell.w = ratio.x;
	playerCell.h = ratio.y;
	playerCell.x = (int)(p->pos.x) * ratio.x + offset.x;
	playerCell.y = (int)(p->pos.y) * ratio.y + offset.y;
	HighlightCell(renderer, playerCell, ROT_GREEN, SAD_GRAY);

	//laser (DDA)
	int wallSide; //not used here
	DDA(p->dir, *p, renderer, &wallSide, ratio, offset);
}

void RotateMatrix(fVector *dir, float angle)
{
	float oldDirX = dir->x;
	float oldDirY = dir->y;
	dir->x = (float)(oldDirX * cos(angle) - oldDirY * sin(angle));
	dir->y = (float)(oldDirX * sin(angle) + oldDirY * cos(angle));
}

void HandlePlayerMovement(const bool* keystate, Player* p)
{
	fVector moveDir = { 0, 0 };

	if (keystate[SDL_SCANCODE_W]) moveDir = p->dir;
	if (keystate[SDL_SCANCODE_S]) moveDir = ScaleVector(p->dir, -1);
	if (keystate[SDL_SCANCODE_D]) moveDir = perpVectorClockwise(p->dir);
	if (keystate[SDL_SCANCODE_A]) moveDir = perpVectorCounterClockwise(p->dir);

	fVector delta = { moveDir.x * p->speed.x, moveDir.y * p->speed.y };

	if (worldMap[(int)(p->pos.y + delta.y)][(int)p->pos.x] == 0) p->pos.y += delta.y;
	if (worldMap[(int)p->pos.y][(int)(p->pos.x + delta.x)] == 0) p->pos.x += delta.x;
}

void HandleRotation(const bool* keystate, Player* p, Camera* cam)
{
	if (keystate[SDL_SCANCODE_LEFT])
	{
		RotateMatrix(&(p->dir), p->rotSpeed);
		RotateMatrix(&(cam->dir), p->rotSpeed);
	}
	if (keystate[SDL_SCANCODE_RIGHT])
	{
		RotateMatrix(&(p->dir), -p->rotSpeed);
		RotateMatrix(&(cam->dir), -p->rotSpeed);
	}
	//update camera
	cam->leftmostRay = RotationMatrix(p->dir, cam->angle / 2.0f);
	cam->rightmostRay = RotationMatrix(p->dir, -cam->angle / 2.0f);
}

fVector RotationMatrix(fVector dir, float angle)
{
	fVector result;
	result.x = (float)(dir.x * cos(angle) - dir.y * sin(angle));
	result.y = (float)(dir.x * sin(angle) + dir.y * cos(angle));
	return result;
}

// DDA algorithm for a single ray, returns the projection distance from the point of
// impact to the camera plane
float DDA(fVector rayDir, Player p, SDL_Renderer* renderer, int* wallSide, fVector ratio, fVector offset)
{
	float deltaX = rayDir.x == 0 ? (float)1e30 : (float)fabs(1.0f / rayDir.x);
	float deltaY = rayDir.y == 0 ? (float)1e30 : (float)fabs(1.0f / rayDir.y);

	iVector currCell = { (int)p.pos.x, (int)p.pos.y };
	float distToNextX = rayDir.x < 0
		? (p.pos.x - currCell.x) * deltaX
		: (currCell.x + 1.0f - p.pos.x) * deltaX;

	float distToNextY = rayDir.y < 0
		? (p.pos.y - currCell.y) * deltaY
		: (currCell.y + 1.0f - p.pos.y) * deltaY;

	float perpWallDist = ShootRay(renderer, rayDir, p, &distToNextX, &distToNextY, wallSide, deltaX, deltaY, currCell, ratio, offset);
	return perpWallDist;
}

// auxiliary function to increase readability
float ShootRay(SDL_Renderer* renderer, fVector rayDir, Player p, float* distX, float* distY, int* sideHit,
	float deltaX, float deltaY, iVector currCell, fVector ratio, fVector offset)
{
	int stepX, stepY;
	if (rayDir.x > 0) stepX = 1; else stepX = -1;
	if (rayDir.y > 0) stepY = 1; else stepY = -1;

	bool hit = false;
	while (!hit)
	{
		if (*distX < *distY)
		{
			*distX += deltaX;
			currCell.x += stepX;
			*sideHit = NS;
		}
		else
		{
			*distY += deltaY;
			currCell.y += stepY;
			*sideHit = EW;
		}

		SDL_FRect rayCell;
		rayCell.w = ratio.x;
		rayCell.h = ratio.y;
		rayCell.x = (int)(currCell.x) * ratio.x + offset.x;
		rayCell.y = (int)(currCell.y) * ratio.y + offset.y;

		//only render the ray poiting the same way as the player's direction
		if (rayDir.x == p.dir.x && rayDir.y == p.dir.y)
			HighlightCell(renderer, rayCell, GHOST_GRAY, SAD_GRAY);

		if (worldMap[currCell.y][currCell.x] != 0)
		{
			hit = true;
			if (rayDir.x == p.dir.x && rayDir.y == p.dir.y)
				HighlightCell(renderer, rayCell, FIRE_ORANGE, ROT_GREEN);
		}
	}

	float perpWallDist;
	if (*sideHit == NS)
		perpWallDist = *distX - deltaX;
	else
		perpWallDist = *distY - deltaY;

	return perpWallDist;
}

fVector DetermineRayDir(float angle, fVector leftmostRay)
{
	fVector result = RotationMatrix(leftmostRay, angle);
	return result;
}

//Vector operations

fVector AddVectors(fVector a, fVector b)
{
	fVector result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}

fVector ScaleVector(fVector v, float scale)
{
	fVector result;
	result.x = v.x * scale;
	result.y = v.y * scale;
	return result;
}

fVector perpVectorClockwise(fVector v)
{
	fVector result;
	result.x = -v.y;
	result.y = v.x;
	return result;
}

fVector perpVectorCounterClockwise(fVector v)
{
	fVector result;
	result.x = v.y;
	result.y = -v.x;
	return result;
}

float Norma(fVector vect)
{
	return (float)sqrt(pow(vect.x, 2) + pow(vect.y, 2));
}
