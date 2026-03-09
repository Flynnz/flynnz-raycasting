#include "render.h"
#define PI 3.14159265

SDL_Color SAD_GRAY = { 85, 85, 90, 255 };
SDL_Color GHOST_GRAY = { 110, 110, 110, 55 };
SDL_Color LIGHT_GRAY = { 95, 95, 95, 255 };
SDL_Color COOL_ORANGE = { 155, 55, 5, 255 };
SDL_Color WEIRD_BLACK = { 20, 30, 20, 255 };
SDL_Color FIRE_RED = { 150, 50, 20, 255 };
SDL_Color FREEZE_BLUE = { 20, 50, 150, 255 };
SDL_Color HOLLOW_PURPLE = { 150, 20, 150, 255 };
SDL_Color ROT_GREEN = { 15, 130, 20, 255 };


int gridState = showGrid;
float mapToScreenX = (float)SCREENWIDTH / MAPWIDTH;
float mapToScreenY = (float)SCREENHEIGHT/ MAPHEIGHT;

void RenderBackground(SDL_Renderer* renderer)
{
	SetRenderColor(renderer, WEIRD_BLACK);
	SDL_RenderClear(renderer);
}

void RenderMap(SDL_Renderer* renderer)
{
	int row, col;
	for (row = 0; row < MAPHEIGHT; row++)
	{
		for (col = 0; col < MAPWIDTH; col++)
		{
			if (worldMap[row][col] > 0)
				RenderCell(renderer, row, col, WEIRD_BLACK, COOL_ORANGE);
			else
				RenderCell(renderer, row, col, SAD_GRAY, LIGHT_GRAY);
		}
	}
}

void SetRenderColor(SDL_Renderer* renderer, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void RenderCell(SDL_Renderer* renderer, int row, int col, SDL_Color border, SDL_Color inner)
{
	SDL_FRect cell;
	cell.w = (float)SCREENWIDTH / MAPWIDTH;
	cell.h = (float)SCREENHEIGHT / MAPHEIGHT;
	cell.x = (float)(col * mapToScreenX);
	cell.y = (float)(row * mapToScreenY);

	SetRenderColor(renderer, inner);
	SDL_RenderFillRect(renderer, &cell);
	
	if (gridState == showGrid)
	{
		SetRenderColor(renderer, border);
		SDL_RenderRect(renderer, &cell);
	}
}

void RenderPlayer(SDL_Renderer* renderer, Player* p)
{
	RenderLaser(renderer, p);
	RenderDirection(renderer, p);
	RenderPlayerBody(renderer, p);
}

void RenderPlayerBody(SDL_Renderer* renderer, Player* p)
{
	SDL_FRect hitbox;
	hitbox.w = (SCREENWIDTH / MAPWIDTH) / 3;
	hitbox.h = (SCREENHEIGHT / MAPHEIGHT) / 3;
	//tricks the renderer to start from the center
	hitbox.x = (float)((p->pos.x) * mapToScreenX - hitbox.w / 2);
	hitbox.y = (float)((p->pos.y) * mapToScreenY - hitbox.h / 2);

	SetRenderColor(renderer, p->color);
	//ill implement square rotation in the future
	SDL_RenderFillRect(renderer, &hitbox);
}

void RenderDirection(SDL_Renderer* renderer, Player* p)
{
	fVector start, end;
	start.x = p->pos.x * mapToScreenX;
	start.y = p->pos.y * mapToScreenY;
	end.x = start.x + p->dir.x * mapToScreenX;
	end.y = start.y + p->dir.y * mapToScreenY;

	float tipLenght;
	tipLenght = (float)1 / 4 * mapToScreenX;
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

void RenderLaser(SDL_Renderer* renderer, Player* p)
{
	fVector start, end;
	start.x = p->pos.x * mapToScreenX;
	start.y = p->pos.y * mapToScreenY;
	end.x = start.x + p->dir.x * SCREENWIDTH;
	end.y = start.y + p->dir.y * SCREENHEIGHT;

	SetRenderColor(renderer, FIRE_RED);
	SDL_RenderLine(renderer, start.x, start.y, end.x, end.y);
}

void RenderCamera(SDL_Renderer* renderer, Player* p)
{
	//limiting rays
	fVector leftmostRay, rightmostRay, endLeft, endRight;
	fVector start = { p->pos.x * mapToScreenX, p->pos.y * mapToScreenY };
	fVector len = { (float)SCREENWIDTH, (float)SCREENHEIGHT };
	leftmostRay = ScaleVector(RotationMatrix(p->dir, (float)PI / 6), len.x);
	endLeft = AddVectors(leftmostRay, start);
	rightmostRay = ScaleVector(RotationMatrix(p->dir, -(float)PI / 6), len.y);
	endRight = AddVectors(rightmostRay, start);

	SetRenderColor(renderer, HOLLOW_PURPLE);
	SDL_RenderLine(renderer, start.x, start.y, endLeft.x, endLeft.y);
	SDL_RenderLine(renderer, start.x, start.y, endRight.x, endRight.y);
	
	//camera plane
	fVector dirEnd = AddVectors(p->dir, p->pos);
	dirEnd.x *= mapToScreenX;
	dirEnd.y *= mapToScreenY;
	fVector cameraEnd1 = AddVectors(dirEnd, ScaleVector(p->camera, SCREENWIDTH));
	fVector cameraEnd2 = AddVectors(dirEnd, ScaleVector(p->camera, -SCREENWIDTH));

	SetRenderColor(renderer, ROT_GREEN);
	SDL_RenderLine(renderer, dirEnd.x, dirEnd.y, cameraEnd1.x, cameraEnd1.y);
	SDL_RenderLine(renderer, dirEnd.x, dirEnd.y, cameraEnd2.x, cameraEnd2.y);
}

void RenderGridOverlap(SDL_Renderer *renderer, Player *p)
{
	//player (!refactor this part into "HighLightCell()")
	SDL_FRect playerCell;
	playerCell.w = mapToScreenX;
	playerCell.h = mapToScreenY;
	playerCell.x = (int)(p->pos.x) * mapToScreenX;
	playerCell.y = (int)(p->pos.y) * mapToScreenY;

	SetRenderColor(renderer, GHOST_GRAY);
	SDL_RenderFillRect(renderer, &playerCell);
	SetRenderColor(renderer, SAD_GRAY);
	SDL_RenderRect(renderer, &playerCell);

	//laser (DDA)
	//...
}

void RotateMatrix(fVector *dir, float angle)
{
	float oldDirX = dir->x;
	float oldDirY = dir->y;
	dir->x = (float)(oldDirX * cos(angle) - oldDirY * sin(angle));
	dir->y = (float)(oldDirX * sin(angle) + oldDirY * cos(angle));
}

fVector RotationMatrix(fVector dir, float angle)
{
	fVector result;
	result.x = (float)(dir.x * cos(angle) - dir.y * sin(angle));
	result.y = (float)(dir.x * sin(angle) + dir.y * cos(angle));
	return result;
}


void DDA(fVector ray, fVector rayDir, Player p)
{
	int step = 1;
	iVector playerCell = { (int)p.pos.x, (int)p.pos.y };
	float deltaX = rayDir.x == 0 ? 1e30 : rayDir.y / rayDir.x;
	float deltaY = rayDir.y == 0 ? 1e30 : rayDir.x / rayDir.y;

	//find sideDistX and Y
	float sideDistX = fabs(playerCell.x - p.pos.x) * deltaX;
	float sideDistY = fabs(playerCell.y - p.pos.y) * deltaY;
	
	if (p.dir.x > 0)
		sideDistX += deltaX;
	if (p.dir.y > 0)
		sideDistY += deltaY;

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

