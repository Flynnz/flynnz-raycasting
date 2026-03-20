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

const iVector startPos = { 1, 1};
fVector dir = { 0, 1 };

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

	//fps counter setup (I let Claude handle this part)
	Uint64 fpsLastTime = SDL_GetTicks();
	int    fpsFrames = 0;
	float  fps = 0.0f;
	char   fpsText[32];

	
	Player p = InitPlayer(WEIRD_BLACK, startPos, dir, (float)0.05, (float)0.02);

	//setting up camera TODO: refactor into a function, refactor camera Struct type
	fVector leftmostRay, rightmostRay;
	leftmostRay = RotationMatrix(p.dir, (float)PI / 6);
	rightmostRay = RotationMatrix(p.dir, -(float)PI / 6);
	float angle = (float)acos((float)fabs(leftmostRay.x) / Norma(leftmostRay));

	//-----------------------
	//       Main loop
	//-----------------------
	bool done = false;
	while (!done)
	{
		leftmostRay = RotationMatrix(p.dir, (float)PI / 6);
		rightmostRay = RotationMatrix(p.dir, -(float)PI / 6);

		SDL_RenderClear(renderer);

		RenderMap(renderer);

		// ── FPS calculation ──────────────────────────────────────────
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
		// ─────────────────────────────────────────────────────────────

		for (float x = 0; x < angle; x += 0.005f)
		{
			fVector DDAdir = DetermineRayDir(x, rightmostRay);

			//printf("angle: %f, dir: (%f, %f)\n", x, DDAdir.x, DDAdir.y);
			DDA(DDAdir, p, renderer);
		}

		if (gridState == showGrid)
		{
			RenderGridOverlap(renderer, &p);
			RenderCamera(renderer, &p, leftmostRay, rightmostRay);
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
		//TO DO: resolve warning (different 'const' qualifiers)
		HandlePlayerMovement(keystate, &p);
		HandleRotation(keystate, &p);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}