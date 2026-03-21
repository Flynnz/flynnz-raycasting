#include "render.h"

#define PI 3.14159265

int worldMap[MAPWIDTH][MAPHEIGHT] =
{
  {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,5,5,5,5,5,5,5,5},
  {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,5},
  {3,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
  {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
  {3,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,5},
  {3,0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,5,5,0,5,5,5,5,5},
  {3,0,0,0,0,0,0,6,0,0,0,6,0,0,0,6,5,0,0,0,5,5,5,2},
  {3,0,0,0,0,0,0,6,0,0,0,0,0,0,0,6,5,0,0,0,0,0,0,7},
  {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,2},
  {3,0,0,0,0,0,0,6,0,0,0,0,0,0,0,6,5,0,0,0,0,0,0,7},
  {3,0,0,0,0,0,0,6,0,0,0,0,0,0,0,6,5,0,0,0,5,5,5,2},
  {3,0,0,0,0,0,0,6,6,6,6,0,6,6,6,6,5,5,5,5,5,5,5,2},
  {7,7,7,7,7,7,7,7,7,7,7,0,7,7,7,7,7,7,7,7,7,7,7,7},
  {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3},
  {7,7,7,7,7,7,0,7,7,7,7,0,7,7,7,7,7,7,7,7,7,7,7,7},
  {3,3,3,3,3,3,0,3,3,3,7,0,7,1,1,1,1,1,1,1,4,4,4,4},
  {3,0,0,0,0,0,0,0,0,3,7,0,7,1,0,0,0,0,0,1,0,0,0,1},
  {3,0,0,0,0,0,0,0,0,0,0,0,7,1,0,0,0,0,0,1,0,0,0,1},
  {3,0,0,0,0,0,0,0,0,3,7,0,7,1,0,0,0,0,0,1,1,0,1,1},
  {3,0,0,0,7,0,0,0,0,3,7,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {3,0,0,0,0,0,0,0,0,3,7,0,7,1,0,0,0,0,0,1,1,0,1,1},
  {3,0,0,0,7,0,0,0,0,3,7,0,7,1,0,0,0,0,0,1,0,0,0,1},
  {3,0,0,0,0,0,0,0,0,3,7,0,7,1,0,0,0,0,0,1,0,0,0,1},
  {3,3,3,3,3,3,3,3,3,3,2,2,2,1,1,1,1,1,1,4,4,4,4,4}
};

const iVector startPos = { 4, 4 };
const fVector startDir = { 0, 1 };
const float moveSpeed = 0.05f;
const float rotateSpeed = 0.02f;
const float cameraSpan = (float)PI / 3.5f;		//angle

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

	Player p = InitPlayer(WEIRD_BLACK, startPos, startDir, moveSpeed, rotateSpeed);
	//TOFIX: implement customizable camera plane distance
	Camera cam = InitCamera(p, cameraSpan);
	fVector mapStartPos, mapToScreenRatio;
	SetScaleAndOffset(&mapStartPos, &mapToScreenRatio);

	//─────────────────────────────────────────────────────────
	//fps counter setup (I let Claude handle this part)
	//─────────────────────────────────────────────────────────
	Uint64 fpsLastTime = SDL_GetTicks();
	int    fpsFrames = 0;
	float  fps = 0.0f;
	char   fpsText[32];

	//─────────────────────────────────────────────────────────
	//       Main loop
	//─────────────────────────────────────────────────────────
	bool done = false;
	while (!done)
	{
		SDL_RenderClear(renderer);

	// ── DDA ───────────────────────────────────────────────────────
		//TODO: more refactoring
		for (float x = 0; x < cam.angle; x += 0.0001f)
		{
			fVector DDAdir = DetermineRayDir(x, cam.rightmostRay);

			int wallSide;
			float perpWallDist = DDA(DDAdir, p, renderer, &wallSide, mapToScreenRatio, mapStartPos);
			
			float wallHeight = SCREENHEIGHT / perpWallDist;

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
		SetScaleAndOffset(&mapStartPos, &mapToScreenRatio);
		HandlePlayerMovement(keystate, &p);
		HandleRotation(keystate, &p, &cam);
	// ── FPS calculation ─────────────────────────────────────────
		fpsFrames++;
		CountFPS(renderer, fpsLastTime, fpsFrames, fps, fpsText);
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