#include "pch.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "GlHeaders.h"
#include "GlmIncludes.h"

#include <SDL2/SDL.h>
#include <iostream>


#include "Application.h"
#include "Log.h"

#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#define PROGRAM_NAME "Draw Commands"


int main(int argc, char *argv[])
{
	SDL_Window *mainwindow; /* Our window handle */
	SDL_GLContext maincontext; /* Our opengl context handle */
	int width = 1024;
	int height = 758;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) 
	{
		printf("Unable to initialize SDL");
		return 1;
	}

	/* Request opengl 4.4 context. */
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	/* Turn on double buffering with a 24bit Z buffer.
	* You may need to change this to 16 or 32 for your system */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	mainwindow = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (!mainwindow)
	{ 
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	maincontext = SDL_GL_CreateContext(mainwindow);

#if defined(_WIN32)
	GLenum rev;
	glewExperimental = GL_TRUE;
	rev = glewInit();

	if (GLEW_OK != rev)
	{
		std::cout << "Error: " << glewGetErrorString(rev) << std::endl;
		exit(1);
	}
	else 
	{
		std::cout << "GLEW Init: Success!" << std::endl;
	}
#endif


	SDL_GL_SetSwapInterval(0);

	bool quit = false;

	Application app;
	if (!app.init(mainwindow))
	{
		return 1;
	}
	app.reshape(width, height);

	SDL_Event event;

	auto lastTick = SDL_GetTicks();

	while (!quit)
	{
		app.render();

		const float dt = (SDL_GetTicks() - lastTick) / 1000.f;
		lastTick = SDL_GetTicks();
		SDL_PumpEvents();
		app.update(dt);
		
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				quit = true;

			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
					quit = true;

				app.keyEvent(event.key.keysym.sym, true);
			}
			else if (event.type == SDL_KEYUP)
			{
				app.keyEvent(event.key.keysym.sym, false);
			}

			if (event.type == SDL_MOUSEMOTION)
			{
				app.updateMouseView(event.motion.x, event.motion.y);
			}

			if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					int w, h;
					SDL_GetWindowSize(mainwindow, &w, &h);
					app.reshape(w, h);
				}
			}
		}
	}

	app.finish();

	/* Delete our opengl context, destroy our window, and shutdown SDL */
	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(mainwindow);
	SDL_Quit();

	return 0;
}

