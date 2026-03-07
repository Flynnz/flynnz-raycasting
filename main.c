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
  {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,1},
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

	iVector startPos = { MAPWIDTH / 2, MAPHEIGHT/ 2};
	
	Player p;
	p.color = WEIRD_BLACK;
	p.pos.x = startPos.x;
	p.pos.y = startPos.y;
	p.dir.x = 0;
	p.dir.y = 1;
	p.speed.x = 0.1;
	p.speed.y = 0.1;
	p.rotSpeed = 0.02;

	//-----------------------
	//       Main loop
	//-----------------------
	bool done = false;
	while (!done)
	{
		SDL_RenderClear(renderer);
		RenderMap(renderer);

		RenderDirection(renderer, &p);
		RenderPlayerBody(renderer, &p);
		
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
		{
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
			if (worldMap[(int)p.pos.y][(int)(p.pos.x - p.dir.y * p.speed.x)] == 0) { p.pos.x += -p.dir.y * p.speed.x; }
			if (worldMap[(int)(p.pos.y - p.dir.y * p.speed.y)][(int)p.pos.x] == 0) { p.pos.y += -p.dir.x * p.speed.y; }

		}
		if (keystate[SDL_SCANCODE_A])
		{
			if (worldMap[(int)p.pos.y][(int)(p.pos.x + p.dir.y * p.speed.x)] == 0) { p.pos.x -= -p.dir.y * p.speed.x; }
			if (worldMap[(int)(p.pos.y + p.dir.y * p.speed.y)][(int)p.pos.x] == 0) { p.pos.y -= -p.dir.x * p.speed.y; }

		}

		//rotation
		if (keystate[SDL_SCANCODE_LEFT])
			RotationMatrix(&(p.dir), p.rotSpeed);
		if (keystate[SDL_SCANCODE_RIGHT])
			RotationMatrix(&(p.dir), -p.rotSpeed);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}