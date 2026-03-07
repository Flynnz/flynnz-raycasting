#include "render.h"
#define PI 3.14159265

SDL_Color SAD_GRAY = { 85, 85, 90, 255 };
SDL_Color LIGHT_GRAY = { 95, 95, 95, 255 };
SDL_Color COOL_ORANGE = { 155, 55, 5, 255 };
SDL_Color WEIRD_BLACK = { 20, 20, 20, 255 };
SDL_Color FIRE_RED = { 150, 50, 20, 255 };
int gridState = showGrid;
float mapToScreenX = SCREENWIDTH / MAPWIDTH;
float mapToScreenY = SCREENHEIGHT/ MAPHEIGHT;

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
	cell.w = SCREENWIDTH / MAPWIDTH;
	cell.h = SCREENHEIGHT / MAPHEIGHT;
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
	end.x = start.x + p->dir.x * mapToScreenX * 3;
	end.y = start.y + p->dir.y * mapToScreenY * 3;

	float tipLenght;
	tipLenght = (float)1 / 3 * mapToScreenX;
	fVector tipDir = RotationMatrix(p->dir, PI/4);
	fVector tipVector = scaleVector(tipDir, -1 * tipLenght);
	fVector tipEnd1 = AddVectors(end, tipVector);
	fVector tipEnd2 = AddVectors(end, perpVectorCounterClockwise(tipVector));

	SetRenderColor(renderer, FIRE_RED);

	//arrow body
	SDL_RenderLine(renderer, start.x, start.y, end.x, end.y);

	//arrow tip
	SDL_RenderLine(renderer, end.x, end.y, tipEnd1.x, tipEnd1.y);
	SDL_RenderLine(renderer, end.x, end.y, tipEnd2.x, tipEnd2.y);
}

void RotateMatrix(fVector *dir, float angle)
{
	float oldDirX = dir->x;
	float oldDirY = dir->y;
	dir->x = oldDirX * cos(angle) - oldDirY * sin(angle);
	dir->y = oldDirX * sin(angle) + oldDirY * cos(angle);
}

fVector RotationMatrix(fVector dir, float angle)
{
	fVector result;
	result.x = dir.x * cos(angle) - dir.y * sin(angle);
	result.y = dir.x * sin(angle) + dir.y * cos(angle);
	return result;
}

fVector AddVectors(fVector a, fVector b)
{
	fVector result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}

fVector scaleVector(fVector v, float scale)
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

