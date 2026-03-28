#include "render.h"

#define PI 3.14159265

int worldMap[MAPWIDTH][MAPHEIGHT] =
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
  {1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,3,3,3,3,3,3,3,3,3,1,1,0,1,1,1,1,1},
  {1,0,0,0,0,0,0,3,0,0,0,3,0,0,0,3,1,0,0,0,1,1,1,1},
  {1,0,0,0,0,0,0,3,0,0,0,0,0,0,0,3,1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
  {1,0,0,0,0,0,0,3,0,0,0,0,0,0,0,3,1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,3,0,0,0,0,0,0,0,3,1,0,0,0,1,1,1,1},
  {1,0,0,0,0,0,0,3,3,3,3,0,3,3,3,3,1,1,1,1,1,1,1,1},
  {2,2,0,2,2,2,2,2,2,2,2,0,2,2,2,2,2,2,2,2,2,2,2,2},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {2,2,0,2,2,2,0,2,2,2,2,0,2,2,2,2,2,2,2,2,2,2,2,2},
  {1,1,1,1,1,1,0,1,1,1,2,0,2,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,1,2,0,2,1,0,0,0,0,0,1,0,0,0,1},
  {1,0,0,0,2,0,0,0,0,0,0,0,2,1,0,0,0,0,0,1,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1,2,0,2,1,0,0,0,0,0,1,1,0,1,1},
  {1,0,0,0,2,0,0,0,0,1,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1,2,0,2,1,0,0,0,0,0,1,1,0,1,1},
  {1,0,0,0,2,0,0,0,0,1,2,0,2,1,0,0,0,0,0,1,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,1,2,0,2,1,0,0,0,0,0,1,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1}
};

const iVector startPos = { 4, 4 };
const fVector startDir = { 0, 1 };
const float rotateSpeed = 0.02f;
float moveSpeed = 0.025f;

const float FOV = 160;
const float angleProgression = 0.0001f;

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
	const float cameraSpan = (float)atan(FOV/100.0f);		//angle

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
		for (float x = 0; x < cam.angle; x += angleProgression)
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
		float actualSpeed = moveSpeed;

		if (event.type == SDL_EVENT_QUIT)
			done = true;
		if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_G)
			gridState = (gridState == noGrid) ? showGrid : noGrid;
		if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_TAB)
			renderState = (renderState == twoD) ? threeD : twoD;
	// ──────────────────────────────────────────────────────────────


	// ── Keystate inputs ───────────────────────────────────────────
		const bool* keystate = SDL_GetKeyboardState(NULL);
		if (keystate[SDL_SCANCODE_ESCAPE])
			done = true;
		if (keystate[SDL_SCANCODE_LCTRL])
			actualSpeed = moveSpeed * 2;
		else
			actualSpeed = moveSpeed;
	// ──────────────────────────────────────────────────────────────


	// ── Display info ──────────────────────────────────────────────
		char instructions[64], coords[24], viewState[24], gridONOFF[24];
		snprintf(instructions, 64, "-quit: ESC  -run: CTRL  -switch view: TAB  -grid ON/OFF: G");
		SDL_SetRenderDrawColor(renderer, 175, 175, 175, 255); //gray
		SDL_RenderDebugText(renderer, SCREENWIDTH - 485, SCREENHEIGHT - 16, instructions);

		snprintf(coords, 24, "x:%.1f y:%.1f", p.pos.x, p.pos.y);
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); //yellow
		SDL_RenderDebugText(renderer, 100 , 4 , coords);

		snprintf(viewState, 24, "view:%d", renderState);
		SDL_RenderDebugText(renderer, 210 , 4 , viewState);
		
		if (gridState == noGrid)
			snprintf(gridONOFF, 24, "grid:OFF");
		else
			snprintf(gridONOFF, 24, "grid:ON");
		SDL_RenderDebugText(renderer, 280 , 4 , gridONOFF);

	// ──────────────────────────────────────────────────────────────


	// ── Updates ───────────────────────────────────────────────────
		SetScaleAndOffset(&mapStartPos, &mapToScreenRatio);
		HandlePlayerMovement(keystate, &p);
		HandleRotation(keystate, &p, &cam);
		UpdatePlayerSpeed(&p, actualSpeed),
	// ── FPS calculation ─────────────────────────────────────────
		fpsFrames++;
		CountFPS(renderer, fpsLastTime, fpsFrames, fps, fpsText);

		if (renderState == threeD)
			SetRenderColor(renderer, LIGHT_GRAY);
	// ──────────────────────────────────────────────────────────────
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

	return 0;
}