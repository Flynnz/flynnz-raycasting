#include "render.h"

#include <stdio.h>

#define PI 3.14159265

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

const iVector startPos = { 4, 4 };
fVector dir = { 0, 1 };

int main(void)
{
	//─────────────────────────────────────────────────────────
	//     Initializing
	//─────────────────────────────────────────────────────────
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("window", SCREENWIDTH, SCREENHEIGHT, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetRenderVSync(renderer, 1);
	SDL_Event event;

	fVector mapStartPos, mapToScreenRatio;
	//TODO: refactor speed and angle speed with a variable
	Player p = InitPlayer(WEIRD_BLACK, startPos, dir, (float)0.05, (float)0.02);
	//TOFIX: camera "lines" arent displaying correctly, implement customizable camera plane distance
	Camera cam = InitCamera(p, (float)PI / 5.5f);

	//─────────────────────────────────────────────────────────
	//fps counter setup (I let Claude handle this part)
	//─────────────────────────────────────────────────────────
	Uint64 fpsLastTime = SDL_GetTicks();
	int    fpsFrames = 0;
	float  fps = 0.0f;
	char   fpsText[32];

	if (renderState == twoD)
	{
		mapStartPos.x = 0;
		mapStartPos.y = 0;
		mapToScreenRatio.x = mapToScreenX;
		mapToScreenRatio.y = mapToScreenY;
	}
	else
	{
		mapStartPos.x = (int)SCREENWIDTH / 24;
		mapStartPos.y = (int)SCREENHEIGHT / 24;
		mapToScreenRatio.x = mapToScreenX / 4;
		mapToScreenRatio.y = mapToScreenY / 4;
	}

	fVector test1, test2;
	test1 = cam.leftmostRay;
	test2 = cam.rightmostRay;

	//─────────────────────────────────────────────────────────
	//       Main loop
	//─────────────────────────────────────────────────────────
	bool done = false;
	while (!done)
	{
		SDL_RenderClear(renderer);

	// ── DDA ───────────────────────────────────────────────────────
		//TODO: more refactoring
		for (float x = 0; x < cam.angle; x += 0.001f)
		{
			fVector DDAdir = DetermineRayDir(x, cam.rightmostRay);

			int wallSide;
			float perpWallDist = DDA(DDAdir, p, renderer, &wallSide, mapToScreenRatio, mapStartPos);
			
			float wallHeight = SCREENHEIGHT / perpWallDist ;

	// ── Render 3D ───────────────────────────────────────────────
			if (renderState == threeD)
			{
				float wallTop, wallBottom;
				wallTop = (SCREENHEIGHT + wallHeight) / 2;
				if (wallTop >= SCREENHEIGHT) wallTop = SCREENHEIGHT - 1;
				wallBottom = (SCREENHEIGHT - wallHeight) / 2;
				if (wallTop < 0) wallBottom = 0;

				float wallX = SCREENWIDTH / cam.angle * x;

				if (wallSide == NS)
					SetRenderColor(renderer, FIRE_ORANGE);
				else
					SetRenderColor(renderer, COAL_ORANGE);

				SDL_RenderLine(renderer, wallX, wallTop, wallX, wallBottom);
			}
		}
	// ──────────────────────────────────────────────────────────────

		
	// ── Render 2D ─────────────────────────────────────────────────

		RenderMap(renderer, mapStartPos, mapToScreenRatio);

		if (gridState == showGrid)
		{
			RenderGridOverlap(renderer, &p, mapToScreenRatio, mapStartPos);
			if (renderState == twoD)
			{
				RenderCamera(renderer, &p, cam, mapToScreenRatio, mapStartPos);
				RenderLaser(renderer, &p, mapToScreenRatio, mapStartPos);
			}
		}
		RenderPlayer(renderer, &p, mapToScreenRatio, mapStartPos);
	// ──────────────────────────────────────────────────────────────
	
	
	
	// ── FPS calculation ───────────────────────────────────────────
		fpsFrames++;
		Uint64 now = SDL_GetTicks();
		Uint64 elapsed = now - fpsLastTime;

		if (elapsed >= 500)                       // update every 500 ms
		{
			fps = fpsFrames / (elapsed / 1000.0f);
			fpsFrames = 0;
			fpsLastTime = now;
		}
		snprintf(fpsText, sizeof(fpsText), "FPS: %.1f", fps);
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);   // yellow
		SDL_RenderDebugText(renderer, 4, 4, fpsText);
	// ──────────────────────────────────────────────────────────────
	

	// ── Event inputs ──────────────────────────────────────────────
		SDL_PollEvent(&event);
		if (event.type == SDL_EVENT_QUIT)
			done = true;
		if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_G)
			gridState = (gridState == noGrid) ? showGrid : noGrid;
		if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_M)
			renderState = (renderState == twoD) ? threeD : twoD;
	// ──────────────────────────────────────────────────────────────


	// ── Keystate inputs ───────────────────────────────────────────
		const bool* keystate = SDL_GetKeyboardState(NULL);
		if (keystate[SDL_SCANCODE_Q])
			done = true;
	// ──────────────────────────────────────────────────────────────


	// ── Updates ───────────────────────────────────────────────────
		if (renderState == twoD)
		{
			mapStartPos.x = 0;
			mapStartPos.y = 0;
			mapToScreenRatio.x = mapToScreenX;
			mapToScreenRatio.y = mapToScreenY;
		}
		else
		{
			mapStartPos.x = (int)SCREENWIDTH / 24;
			mapStartPos.y = (int)SCREENHEIGHT / 24;
			mapToScreenRatio.x = mapToScreenX / 4;
			mapToScreenRatio.y = mapToScreenY / 4;
		}
		HandlePlayerMovement(keystate, &p);
		HandleRotation(keystate, &p, &cam);
	// ──────────────────────────────────────────────────────────────

		if (renderState == threeD)
			SetRenderColor(renderer, LIGHT_GRAY);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}