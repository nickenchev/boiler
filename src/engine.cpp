#include <SDL_image.h>
#include <iostream>
#include "opengl.h"
#include "engine.h"
#include "spritesheet.h"
#include "part.h"
#include "openglrenderer.h"
#include "entity.h"

#define RENDERER_CLASS OpenGLRenderer

Engine::Engine() : spriteLoader(*this), keys{0}
{
    renderer = std::make_unique<RENDERER_CLASS>(*this);

    lastTime = currentTime = 0;
    frameDelta = 0;
}

void Engine::initialize()
{
    bool success = false;
    
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        win = SDL_CreateWindow("Boiler", 0, 0, RES_WIDTH, RES_HEIGHT, SDL_WINDOW_OPENGL);
        if (win)
        {
            // setup opengl
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

            glContext = SDL_GL_CreateContext(win);
            if (glContext)
            {
                std::cout << " - Using Context: OpenGL " << glGetString(GL_VERSION) << std::endl; 

                // require "experimental" as not all OpenGL features are marked "standard"
                glewExperimental = GL_TRUE;
                if (glewInit() != GLEW_OK)
                {
                    std::cerr << "Error initializing GLEW" << std::endl;
                }
                // glewInit() queries extensions incorrectly, clearing errors here
                glGetError();
                
                IMG_Init(IMG_INIT_PNG);
                success = true;
            }
        }
    }

    if (!success)
    {
        std::cout << "Error Initializing SDL: " << SDL_GetError() << std::endl;
    }
    else
    {
        // initialization was successful
        std::cout << " - Preferred Path: " << SDL_GetPrefPath("ensoft", "sdl_engine") << std::endl;
        std::cout << " - Base Path: " << SDL_GetBasePath() << std::endl;

        // initialize basic engine stuff
        frameInterval = 1.0f / 60.0f; // 60fps

        // compile the default shader program
        program = std::make_unique<ShaderProgram>("shader");
    }
}

void Engine::start(Part *part)
{
    //setup global scaling
    globalScale = glm::vec2(1.5f, 1.5f);

    //store the incoming part and start it
    this->part = part;
    part->start();

    //start processing events
    run();
}

void Engine::run()
{
    running = true;
    while(running)
    {
        //get the delta time
        currentTime = SDL_GetTicks();
        frameDelta += (currentTime - lastTime) / 1000.0f;

        //if its time to process a frame, do it here 
        if (frameDelta >= frameInterval)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch(event.type)
                {
                    case SDL_KEYDOWN:
                    {
                        SDL_Keycode keyCode = event.key.keysym.sym;
                        keys[keyCode] = true;
                        break;
                    }
                    case SDL_KEYUP:
                    {
                        SDL_Keycode keyCode = event.key.keysym.sym;
                        keys[keyCode] = false;
                        break;
                    }
                }
            }
            //update the state and render to screen
            update(frameDelta);
            render(frameDelta);

            frameDelta = 0;
        }
        lastTime = currentTime;
    }
}

void Engine::update(const float delta)
{
    part->handleInput();
    part->update(delta);
}

void Engine::render(const float delta)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderer->render();

    SDL_GL_SwapWindow(win);
}

Engine::~Engine()
{
    if (glContext)
    {
        std::cout << "* Destroying GL Context" << std::endl;
        SDL_GL_DeleteContext(glContext);
    }
    if (win)
    {
        std::cout << "* Destroying Window" << std::endl;
        SDL_DestroyWindow(win);
    }
    SDL_Quit();
}
