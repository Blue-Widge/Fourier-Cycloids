#include "shapes.h"
#include "svgparser.h"
#include "BezierCurve.h"
#include "complex.h"
#include "SDL2/SDL.h"
#include "Fourier.h"
#include <time.h>

int precision = 1;

void doInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
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
	}
}

int main(int argc, char* argv[])
{
#if 0
	xmlDocPtr svgfile = PARSER_LoadSVG("../Cycloide/test.xml");
	xmlNodeShape* shapes = PARSER_GetShapesFromSVG(svgfile);

	SHAPE_Point test[4], testResult[4];
	test[0].x = 50; test[0].y = 50;
	test[1].x = 300; test[1].y = 10;
	test[2].x = 400; test[2].y = 600;
	test[3].x = 1000; test[3].y = 500;
	double** func = getBezierFunction(test[0], test[1], test[2], test[3]);

	testResult[0] = getBezierPoint(func, 0.25);
	testResult[1] = getBezierPoint(func, 0.5);
	testResult[2] = getBezierPoint(func, 0.75);
	testResult[3] = getBezierPoint(func, 1);

	SDL_Window* window = SDL_CreateWindow("BezierCurve", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 720, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	while (1)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		doInput();
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawLineF(renderer, test[0].x, test[0].y, test[1].x, test[1].y);
		SDL_RenderDrawLineF(renderer, test[1].x, test[1].y, test[2].x, test[2].y);
		SDL_RenderDrawLineF(renderer, test[2].x, test[2].y, test[3].x, test[3].y);

		SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
		SHAPE_Point lastPoint = test[0];
		for (int i = 0; i <= precision; ++i)
		{
			SHAPE_Point current = getBezierPoint(func, (double)i/(double)precision);
			SDL_RenderDrawLineF(renderer, lastPoint.x, lastPoint.y, current.x, current.y);
			lastPoint = current;
		}
		SDL_RenderPresent(renderer);
		SDL_Delay(0);
	}

	freeBezierFunction(func);
#else
	srand(time(NULL));

	struct Complex_s a = createComplex(3, -6);
	struct Complex_s b = createComplex(12, 1);
	struct Complex_s c = createComplex(0, 7);
	struct Complex_s d = createComplex(4, -2);
	struct ComplexList_s* list = createComplexList(&a);
	addObjectComplexList(&list, &b);
	addObjectComplexList(&list, &c);
	addObjectComplexList(&list, &d);
	struct Complex_s result1 = addComplexList(list);
	struct Complex_s result2 = subComplexList(list);
	struct Complex_s result3 = multiplyComplexList(list);
	struct Complex_s result4 = divideComplexList(list);
	printComplex(result1, 4);
	printComplex(result2, 4);
	printComplex(result3, 4);
	printComplex(result4, 4);
	destroyComplexList(list);

	int precision = 10;
	struct Complex_s* tab = (struct Complex_s*) calloc(precision, sizeof(struct Complex_s));

	for (int i = 0; i < precision; ++i)
	{
		tab[i].m_real = rand() % (2 - 0 + 1) + 0;
		tab[i].m_imaginary = rand() % (2 - 0 + 1) + 0;
	}
	struct Circle_s* circleList = (struct Circle_s*)calloc(precision + 1, sizeof(struct Circle_s));
	circleList[0] = createCircle(0, (SHAPE_Point) { 0, 0 }, createComplex(0, 0));

	for (int i = precision / 2 * -1; i < 0; ++i)
	{
		circleList[1 + i + (precision / 2)] = createCircle(i, (SHAPE_Point) { tab[i + (precision / 2)].m_real, tab[i + (precision / 2)].m_imaginary }, getCoeff(i, tab[i + (precision / 2)]));
		addCircleList(circleList, &circleList[1 + i + (precision / 2)]);
	}

	for (int i = 1; i <= precision; ++i)
	{
		circleList[i + precision/2] = createCircle(i, (SHAPE_Point) { tab[i + (precision / 2)].m_real, tab[i + (precision / 2)].m_imaginary }, getCoeff(i, tab[i + (precision / 2)]));
		addCircleList(circleList, &circleList[1 + i + (precision / 2)]);
	}

	printf("test\n");
#endif
	return 0;
}