#include <stdbool.h>
#include "shapes.h"
#include "svgparser.h"
#include "BezierCurve.h"
#include "Camera.h"
#include "Menutextures.h"
#include "SDL.h"
#include "SDL_image.h"

int main(int argc, char* argv[])
{
	xmlDocPtr svgfile = PARSER_LoadSVG("../Cycloide/test.xml");
	xmlNodeShape* shapes = PARSER_GetShapesFromSVG(svgfile);


	Camera *camera = NULL;
	camera = (Camera*)calloc(1, sizeof(Camera));
	MenuTextures* textures = NULL;


	camera->screen = (SDL_Rect*)calloc(1, sizeof(SDL_Rect));
	camera->screen->h = 720;
	camera->screen->w = 1024;
	camera->screen->x = 0;
	camera->screen->y = 0;

	int screenw = camera->screen->w;
	int screenh = camera->screen->h;

	SHAPE_Point test[4], testResult[4];
	test[0].x = 50; test[0].y = 50;
	test[1].x = 300; test[1].y = 10;
	test[2].x = 0; test[2].y = 200;
	test[3].x = 2000; test[3].y = 2000;
	double** func = getBezierFunction(test[0], test[1], test[2], test[3]);
	
	testResult[0] = getBezierPoint(func, 0.25);
	testResult[1] = getBezierPoint(func, 0.5);
	testResult[2] = getBezierPoint(func, 0.75);
	testResult[3] = getBezierPoint(func, 1);

	if (!SDL_WasInit(SDL_INIT_EVERYTHING))
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			printf("[-] ERROR - Failed to initialise SDL (%s)\n", SDL_GetError());
			return EXIT_FAILURE;
		}
	}

	if (!IMG_Init(IMG_INIT_PNG))
	{
		printf("[-] ERROR - Failed to initialise SDL_Image (%s)\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	SDL_Window* window = SDL_CreateWindow("BezierCurve", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 720, SDL_WINDOW_OPENGL);
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	bool a = false;
	bool rouleau = false;
	bool rouleau2 = false;
	int inc = 0;
	//Initialisation Cam�ra
	camera->ratiox = 1;
	camera->ratioy = 1;
	camera->ratioX = 1;
	camera->ratioY = 1;

	//Initialisation Textures
	textures = MenuTextures_new(renderer);
	Found(textures, "rouleau");

	//int var_w = textures->rouleau->r->w;
	//int var_h = textures->rouleau->r->h;

	//Cr�ation Cam�ra
	SDL_RenderSetScale(renderer, camera->ratiox, camera->ratioy);
	while (1)
	{
		SDL_SetRenderDrawColor(renderer, 0,0,0, 255);
		SDL_RenderClear(renderer);
		SDL_Event event;
		
		while (SDL_PollEvent(&event))
		{
			int wheel = 0;
			switch (event.type)
			{
			case SDL_MOUSEBUTTONDOWN:
				a = true;
				camera->xp = event.button.x;
				camera->yp = event.button.y;
				if (camera->xp >= 330 && camera->xp <= 360 && rouleau2==false)
					rouleau=true;
				if ((camera->xp >= 330 && camera->xp <= 380) && (camera->yp >= 210 && camera->yp <= 500) && rouleau2==false)
					rouleau = true;

				if (camera->xp >= 0 && camera->yp <= 30 && rouleau == false) 
					rouleau2 = true;
					
				if ((camera->xp >= 0 && camera->xp <= 50) && (camera->yp >= 210 && camera->yp <= 500) && rouleau == false)
					rouleau2 = true;
				printf("\n\n%d %d \n\n", camera->xp, camera->yp);

				break;

			case SDL_MOUSEBUTTONUP:
				a = false;
				camera->xp = event.motion.x;
				camera->yp = event.motion.y;
				Camera_ViewToWorld(&camera, screenw, screenh,  0);
				//Camera_ImageToWorld(camera, &textures, var_h, var_w);
				break;

			case SDL_MOUSEWHEEL:
			
				wheel = event.wheel.y;
				if (wheel == -1) {	
					screenw += 50;
					screenh += 50;
				}
				if (wheel == 1) {	
					screenw -= 50;
					screenh -= 50;
				}
				Camera_ViewToWorld(&camera, screenw, screenh, 1);
				//Camera_ImageToWorld(camera, &textures, var_h,var_w );
				wheel = 0;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					exit(0);
					break;
				case SDLK_UP:
					precision++;
					break;
				case SDLK_DOWN:
					precision -= precision > 1 ? 1 : 0;
					break;
				case SDL_QUIT:
					exit(0);
					break;
				default:
					break;
				}
			}
			if (a == true) {
				camera->x = camera->xp;
				camera->y = camera->yp;
				camera->xp = event.motion.x;
				camera->yp = event.motion.y;
				Camera_ViewToWorld(&camera, screenw, screenh, 0);
				//Camera_ImageToWorld(camera, &textures, var_h, var_w);
			}
			SDL_RenderSetScale(renderer, camera->ratiox, camera->ratioy);
			SDL_RenderSetViewport(renderer, camera->screen);
		}
		SDL_SetRenderDrawColor(renderer, 255,255,255, 255);
		SDL_RenderDrawLineF(renderer, test[0].x, test[0].y, test[1].x, test[1].y);
		SDL_RenderDrawLineF(renderer, test[1].x, test[1].y, test[2].x, test[2].y);
		SDL_RenderDrawLineF(renderer, test[2].x, test[2].y, test[3].x, test[3].y);

		SDL_SetRenderDrawColor(renderer, 255,255,255, 255);
		SHAPE_Point lastPoint = test[0];
		for (int i = 0; i <= precision; ++i)
		{
			SHAPE_Point current = getBezierPoint(func, (double)i/(double)precision);
			if (current.x > camera->screen->x) {
				float sub = current.x - camera->screen->x;
				camera->screen->w += sub;
			}
			if (current.y > camera->screen->y) {
				float sub = current.y - camera->screen->h;
				camera->screen->h += sub;
			}
			SDL_RenderDrawLineF(renderer, lastPoint.x, lastPoint.y, current.x, current.y);
			lastPoint = current;
		}
		

		render(renderer, textures);
		
		Deroulement(textures, rouleau, rouleau2);
		

		SDL_RenderPresent(renderer);
		SDL_Delay(0);
	}

	SDL_Quit();
	freeBezierFunction(func);
	SDL_DestroyRenderer(renderer);
	MenuTextures_free(textures);
}