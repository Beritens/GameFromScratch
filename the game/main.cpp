#include <chrono>
#include <cstdlib>

#include "SDL.h"
#include "SDL_image.h"
#include <glad/glad.h>

#define TWO_ENTITY_MAX (32<<10)
#include "entity.h"
#include "utility.h"
#include "systems.h"
#include "components.h"
#include "inputManager.h"


#include "rendering/Renderer.h"
#include "rendering/VertexBuffer.h"
#include "rendering/VertexBufferLayout.h"
#include "rendering/IndexBuffer.h"
#include "rendering/VertexArray.h"
#include "rendering/Shader.h"
#include <iostream>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>
#include "rendering/GLHelper.h"

#define _USE_MATH_DEFINES

#include <cmath>


// Config
constexpr int   WindowX = 800;
constexpr int   WindowY = 600;
SDL_GLContext g_OpenGLContext = nullptr;

InputManager* g_InputManager;
SDL_Window* g_ApplicationWindow = nullptr;


// -- Events --------------------------

struct QuitEvent {};

struct KeyDown {
    SDL_Keycode key;
    SDL_Scancode scancode;
};
struct KeyUp {
    SDL_Keycode key;
    SDL_Scancode scancode;
};





static void spawn_Map(two::World* world, two::Entity player) {
    for (int i = 0; i < 150; i++) {
        world->pack(world->make_entity(),
            Transform{ {(1.0f*i),5}, {1,1}, 0},
            Follower{ player, 10.0f },
            PhyObj{ {0,0}, true, 1 },
            Sprite{ 1, {0,0}, {1,1}, {0,0} },
            Collider{0.5});
    }


}





// -- World ---------------------------

class MainWorld : public two::World {
public:
    void load() override {
        g_InputManager = new InputManager();

        make_system<PhysicsSystem>();
        make_system<FollowSystem>();
        make_system<ShootSystem>();
        make_system<ParticleSystem>();
        make_system<ControlSystem>();
        make_system<AnimationSystem>();
        make_system<SpriteRenderer>();
        make_system<ItemHandler>();


        bind<KeyDown>(&MainWorld::keydown, this);
        bind<KeyUp>(&MainWorld::keyup, this);
        two::Entity player = make_entity();
        pack(player, Controller{},
            Transform{ {0,0}, {6,6} , 0 },
            PhyObj{ {0,0}, true, 10.f },
            Collider{1.f},
            Sprite{ 0, {0,0}, {1.f/9.f,1}, {0,0} },
            Animation{0,0.f },
            Emitter{},
            ItemHolder{ {1,1,1,0,0}});
        PlayerSkinManager::changePlayerSkin(unpack_one<ItemHolder>());
        pack(make_entity(),
            Transform{ {1,5}, {3,3}, 0 },
            PhyObj{ {0,0}, true, 1 },
            Sprite{ 1, {3.f/10.f,0}, {0.1f,1}, {0,0} },
            Collider{ 0.5 });
        pack(make_entity(),
            Transform{ {5,5}, {3,3}, 0 },
            PhyObj{ {0,0}, true, 1 },
            Sprite{ 1, {4.f / 10.f,0}, {0.1f,1}, {0,0} },
            Collider{ 0.5 });
        //spawn_Map(this, player);
        /*for (int i = 0; i < MaxParticles; ++i) {
            spawn_particle(this, make_entity(), float2(EmitterX, EmitterY));
        }*/
    }

    void update(float dt) override {
        for (auto* system : systems()) {
            system->update(this, dt);
        }
        
    }

    bool keydown(const KeyDown& event) {
        if (event.key == SDLK_SPACE) {
            return true;
        }
        if (event.key == SDLK_w) {
            float2 curI = g_InputManager->GetMovement();
            curI.y += 1;
            g_InputManager->SetMovement(curI);
        }
        if (event.key == SDLK_a) {
            float2 curI = g_InputManager->GetMovement();
            curI.x -= 1;
            g_InputManager->SetMovement(curI);
        }
        if (event.key == SDLK_s) {
            float2 curI = g_InputManager->GetMovement();
            curI.y -= 1;
            g_InputManager->SetMovement(curI);
        }
        if (event.key == SDLK_d) {
            float2 curI = g_InputManager->GetMovement();
            curI.x += 1;
            g_InputManager->SetMovement(curI);
        }
        if (event.key == SDLK_i) {
            float2 curI = g_InputManager->GetShootDir();
            curI.y += 1;
            g_InputManager->SetShootDir(curI);
        }
        if (event.key == SDLK_j) {
            float2 curI = g_InputManager->GetShootDir();
            curI.x -= 1;
            g_InputManager->SetShootDir(curI);
        }
        if (event.key == SDLK_k) {
            float2 curI = g_InputManager->GetShootDir();
            curI.y -= 1;
            g_InputManager->SetShootDir(curI);
        }
        if (event.key == SDLK_l) {
            float2 curI = g_InputManager->GetShootDir();
            curI.x += 1;
            g_InputManager->SetShootDir(curI);
        }
        return false;
    }
    bool keyup(const KeyUp& event) {
        if (event.key == SDLK_SPACE) {
            auto& emitter = unpack_one<Emitter>();
            return true;
        }
        if (event.key == SDLK_w) {
            float2 curI = g_InputManager->GetMovement();
            curI.y -= 1;
            g_InputManager->SetMovement(curI);
        }
        if (event.key == SDLK_a) {
            float2 curI = g_InputManager->GetMovement();
            curI.x += 1;
            g_InputManager->SetMovement(curI);
        }
        if (event.key == SDLK_s) {
            float2 curI = g_InputManager->GetMovement();
            curI.y += 1;
            g_InputManager->SetMovement(curI);
        }
        if (event.key == SDLK_d) {
            float2 curI = g_InputManager->GetMovement();
            curI.x -= 1;
            g_InputManager->SetMovement(curI);
        }
        if (event.key == SDLK_i) {
            float2 curI = g_InputManager->GetShootDir();
            curI.y -= 1;
            g_InputManager->SetShootDir(curI);
        }
        if (event.key == SDLK_j) {
            float2 curI = g_InputManager->GetShootDir();
            curI.x += 1;
            g_InputManager->SetShootDir(curI);
        }
        if (event.key == SDLK_k) {
            float2 curI = g_InputManager->GetShootDir();
            curI.y += 1;
            g_InputManager->SetShootDir(curI);
        }
        if (event.key == SDLK_l) {
            float2 curI = g_InputManager->GetShootDir();
            curI.x -= 1;
            g_InputManager->SetShootDir(curI);
        }
        return false;
    }
};
void InitializeProgram() {
    g_ApplicationWindow = SDL_CreateWindow("THE game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WindowX, WindowY, SDL_WINDOW_OPENGL);
    SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
    if (g_ApplicationWindow == nullptr) {
        std::cout << "SDL_WWindow was not able to be created" << std::endl;
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    g_OpenGLContext = SDL_GL_CreateContext(g_ApplicationWindow);
    if (g_OpenGLContext == nullptr) {
        std::cout << "GL_Context was not able to be created" << std::endl;
        exit(1);
    }
    //initialize Glad Library
    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cout << "GLAD was not initialized" << std::endl;
        exit(1);
    }

    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;


    
}



int main(int, char* []) {
    InitializeProgram();
    MainWorld world;
    world.load();

    auto frame_begin = std::chrono::high_resolution_clock::now();
    auto frame_end = frame_begin;
    bool running = true;
    SDL_Event e;

    world.bind<QuitEvent>([&running](const QuitEvent&) {
        running = false;
        return false;
        });


    //this will hopefully go into anothe file:

    /*float positions[] = {
            -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 1.0f,
    };*/
    

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    
    

    Renderer::Init();

   
    while (running) {
        frame_begin = frame_end;
        frame_end = std::chrono::high_resolution_clock::now();
        auto dt_micro = std::chrono::duration_cast<std::chrono::microseconds>(
            (frame_end - frame_begin)).count();
        float dt = float(double(dt_micro) * 1e-6);

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                world.emit(QuitEvent{});
                break;
            case SDL_KEYDOWN:
                if (e.key.repeat == 0) {
                    world.emit(KeyDown{ e.key.keysym.sym, e.key.keysym.scancode });
                }
                
                break;
            case SDL_KEYUP:
                world.emit(KeyUp{ e.key.keysym.sym, e.key.keysym.scancode });
                break;
            }
        
        }
        Renderer::Clear();
        Renderer::BeginBatch();

        world.update(dt);
        /*for (auto* system : world.systems()) {
            system->draw(&world);
        }*/
        world.collect_unused_entities();


        //this should also go into another file:


        
        Renderer::EndBatch();
        Renderer::Flush();
        
        //Renderer::Draw();
        SDL_GL_SwapWindow(g_ApplicationWindow);

        //testing out dynamic drawing stuff




    }
    SDL_DestroyWindow(g_ApplicationWindow);
    SDL_Quit();
    return 0;
}