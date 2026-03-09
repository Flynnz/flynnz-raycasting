#include "render.h"

#include <stdio.h>

int worldMap[MAPWIDTH][MAPHEIGHT] =
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,2,2,0,0,2,2,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,2,2,0,0,2,2,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int main(void)
{
	//-----------------------
	//     Initializing
	//-----------------------
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("window", SCREENWIDTH, SCREENHEIGHT, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetRenderVSync(renderer, 1);
	SDL_Event event;

	iVector startPos = { 1, 1};
	
	Player p;
	p.color = WEIRD_BLACK;
	p.pos.x = (float)startPos.x;
	p.pos.y = (float)startPos.y;

	p.dir.x = 0;
	p.dir.y = 1;

	p.speed.x = (float)0.05;
	p.speed.y = (float)0.05;
	p.rotSpeed = (float)0.02;

	p.camera = perpVectorClockwise(p.dir);

	//-----------------------
	//       Main loop
	//-----------------------
	bool done = false;
	while (!done)
	{
		SDL_RenderClear(renderer);

		RenderMap(renderer);
		if (gridState == showGrid)
		{
			RenderGridOverlap(renderer, &p);
			RenderCamera(renderer, &p);
			RenderLaser(renderer, &p);
		}
		RenderPlayer(renderer, &p);

		
		//event inputs
		SDL_PollEvent(&event);
		if (event.type == SDL_EVENT_QUIT)
			done = true;
		if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_G)
			gridState = (gridState == noGrid) ? showGrid : noGrid;

		//keystate inputs
		const bool* keystate = SDL_GetKeyboardState(NULL);
		if (keystate[SDL_SCANCODE_Q])
			done = true;
		
		//movement
		if (keystate[SDL_SCANCODE_W])
		{	//refactor with raycasting collision detection and vector sum functions in the future !!!
			if (worldMap[(int)(p.pos.y + p.dir.y * p.speed.y)][(int)p.pos.x] == 0) { p.pos.y += p.dir.y * p.speed.y; }
			if (worldMap[(int)p.pos.y][(int)(p.pos.x + p.dir.x * p.speed.x)] == 0) { p.pos.x += p.dir.x * p.speed.x; }

		}
		if (keystate[SDL_SCANCODE_S])
		{
			if (worldMap[(int)(p.pos.y - p.dir.y * p.speed.y)][(int)p.pos.x] == 0) { p.pos.y -= p.dir.y * p.speed.y; }
			if (worldMap[(int)p.pos.y][(int)(p.pos.x - p.dir.x * p.speed.x)] == 0) { p.pos.x -= p.dir.x * p.speed.x; }

		}
		if (keystate[SDL_SCANCODE_D])
		{
			if (worldMap[(int)(p.pos.y - p.dir.x * p.speed.y)][(int)p.pos.x] == 0) { p.pos.y += -p.dir.x * p.speed.y; }
			if (worldMap[(int)p.pos.y][(int)(p.pos.x - p.dir.y * p.speed.x)] == 0) { p.pos.x += -p.dir.y * p.speed.x; }

		}
		if (keystate[SDL_SCANCODE_A])
		{
			if (worldMap[(int)(p.pos.y + p.dir.x * p.speed.y)][(int)p.pos.x] == 0) { p.pos.y -= -p.dir.x * p.speed.y; }
			if (worldMap[(int)p.pos.y][(int)(p.pos.x + p.dir.y * p.speed.x)] == 0) { p.pos.x -= -p.dir.y * p.speed.x; }

		}

		//camera rotation
		if (keystate[SDL_SCANCODE_LEFT])
		{
			RotateMatrix(&(p.dir), p.rotSpeed);
			RotateMatrix(&(p.camera), p.rotSpeed);
		}
		if (keystate[SDL_SCANCODE_RIGHT])
		{
			RotateMatrix(&(p.dir), -p.rotSpeed);
			RotateMatrix(&(p.camera), -p.rotSpeed);
		}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}