/**
* Author: Andy Ng 
* Assignment: Rise of the AI
* Date due: 2024-03-30, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Entity.hpp"
#include "Map.h"
#include <iostream>
#include <vector>
using namespace std;

#define PLATFORM_COUNT 17
#define ENEMY_COUNT 3
#define MAX_BULLETS 6
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 5

const int SPRITESHEET_DIMENSIONS = 4;

const int CD_QUAL_FREQ    = 44100,  // compact disk (CD) quality frequency
          AUDIO_CHAN_AMT  = 2,
          AUDIO_BUFF_SIZE = 4096;

const char BGM_FILEPATH[] = "assets/Magicdustbin.mp3";
const int  LOOP_FOREVER = -1;  // -1 means loop forever in Mix_PlayMusic; 0 means play once and loop zero times

Mix_Music *g_music;

//const char MAP_TILESET_FILEPATH[] = "assets/tileset.png";

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemies;
//    Map *map;
   
};

GameState gameState;

//unsigned int LEVEL_1_DATA[] =
//{
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
//    2, 2, 2, 2, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2,
//    2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2
//};

SDL_Window* displayWindow;
bool gameIsRunning = true;

float success = 0; // 0 = ongoing, 1 = pass, -1 = fail

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;

GLuint loadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        cout << "Unable to load image.\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void initialize() {
//    SDL_Init(SDL_INIT_VIDEO);
    // Initialising both the video AND audio subsystems
       // We did something similar when we talked about video game controllers
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // Start Audio
    Mix_OpenAudio(
        CD_QUAL_FREQ,        // the frequency to playback audio at (in Hz)
        MIX_DEFAULT_FORMAT,  // audio format
        AUDIO_CHAN_AMT,      // number of channels (1 is mono, 2 is stereo, etc).
        AUDIO_BUFF_SIZE      // audio buffer size in sample FRAMES (total samples divided by channel count)
    );
    
    
    // Similar to our custom function load_texture
    g_music = Mix_LoadMUS(BGM_FILEPATH);

    // This will schedule the music object to begin mixing for playback.
    // The first parameter is the pointer to the mp3 we loaded
    // and second parameter is the number of times to loop.
    Mix_PlayMusic(g_music, LOOP_FOREVER);

    // Set the music to half volume
    // MIX_MAX_VOLUME is a pre-defined constant
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

   
    displayWindow = SDL_CreateWindow("Rise of the AI!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
//    GLuint map_texture_id = loadTexture(MAP_TILESET_FILEPATH);
//    gameState.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 4, 1);  // Look at this beautiful initialisation. That's literally it
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.set_projection_matrix(projectionMatrix);
    program.set_view_matrix(viewMatrix);
    
    glUseProgram(program.get_program_id());
    
    glClearColor(0.17f, 0.13f, 0.07f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    // Initialize Player
    gameState.player = new Entity();
    gameState.player->entityType = PLAYER;
    gameState.player->setPosition(glm::vec3(-4, 3, 0));
    gameState.player->setMovement(glm::vec3(0.0f));
    gameState.player->mAcceleration = glm::vec3(0, -9.81f, 0);
    gameState.player->mSpeed = 2.5f;
    gameState.player->textureID = loadTexture("assets/pokemontrainer.png");
    
    
    gameState.player->trainerWalkingAnimations[gameState.player->LEFT] = new int[4] {4, 5, 6, 7};
    gameState.player->trainerWalkingAnimations[gameState.player->RIGHT] = new int[4] {8, 9, 10, 11};
    gameState.player->trainerWalkingAnimations[gameState.player->UP] = new int[4] {12, 13, 14, 15};
    gameState.player->trainerWalkingAnimations[gameState.player->DOWN] = new int[4] {0, 1, 2, 3};


    gameState.player->indices = gameState.player->trainerWalkingAnimations[gameState.player->DOWN]; // set indices to an animation
    gameState.player->frames = 4;
    gameState.player->index = 0;
    gameState.player->animationTime = 0;
    gameState.player->cols = 4;
    gameState.player->rows = 4;
    
    gameState.player->jumpPower = 6.5f;
    
    // Initialize Bullet
    
//    gameState.bullets = new Entity[MAX_BULLETS];
//    for (int i = 0; i < MAX_BULLETS; ++i){
//        gameState.bullets[i].textureID = loadTexture("assets/fireball.png");
//        gameState.bullets[i].entityType = BULLET;
//        
//    }
    // initialize enemy
    gameState.enemies = new Entity[ENEMY_COUNT];
    gameState.enemies[0].textureID = loadTexture("assets/giratina.png");
    gameState.enemies[0].entityType = ENEMY;
    gameState.enemies[0].setPosition(glm::vec3(4, -2.25, 0));
    gameState.enemies[0].mSpeed = 1;
    gameState.enemies[0].aiType = WAITANDGO;
    gameState.enemies[0].aiState = WALKING;
    gameState.enemies[0].jumpPower = 3.5f;
    gameState.enemies[0].mAcceleration = glm::vec3(0, -9.81, 0);
    
    gameState.enemies[1].textureID = loadTexture("assets/arceus.png");
    gameState.enemies[1].entityType = ENEMY;
    gameState.enemies[1].setPosition(glm::vec3(2.0f, 0.0f, 0));
    gameState.enemies[1].mSpeed = 1;
    gameState.enemies[1].aiType = JUMPER;
    gameState.enemies[1].aiState = JUMPING;
    gameState.enemies[1].jumpPower = 3.5f;
    gameState.enemies[1].mAcceleration = glm::vec3(0, -9.81, 0);
    
    gameState.enemies[2].textureID = loadTexture("assets/dialga.png");
    gameState.enemies[2].entityType = ENEMY;
    gameState.enemies[2].setPosition(glm::vec3(-3.0f, -1.0f, 0));
    gameState.enemies[2].mSpeed = 1;
    gameState.enemies[2].aiType = WALKER;
    gameState.enemies[2].aiState = WALKING;
    gameState.enemies[2].jumpPower = 3.5f;
    gameState.enemies[2].mAcceleration = glm::vec3(0, -9.81, 0);
    
  
    


    
    
    // initialize textures
    gameState.platforms = new Entity[PLATFORM_COUNT]; // pointer to an array with size 17
    GLuint platformTextureID =loadTexture("assets/landingplatform.png");
    
    fontTextureID = loadTexture("assets/font.png");
    
//    platform creation
    for (int i = 0; i < 11; ++i) {
        gameState.platforms[i].entityType = PLATFORM;
        gameState.platforms[i].textureID = platformTextureID;
        gameState.platforms[i].setPosition(glm::vec3(-5 + i, -3.25f, 0));
    }
    
    for (int i = 11; i < 13; ++i) {
        gameState.platforms[i].entityType = PLATFORM;
        gameState.platforms[i].textureID = platformTextureID;
        gameState.platforms[i].setPosition(glm::vec3(1.0 + i - 11, -1.25f, 0));
    }
    
    for (int i = 13; i < 15; ++i) {
        gameState.platforms[i].entityType = PLATFORM;
        gameState.platforms[i].textureID = platformTextureID;
        gameState.platforms[i].setPosition(glm::vec3(-3 + i - 15, 0.0f, 0));
    }
    
    
    for (int i = 15; i < PLATFORM_COUNT; ++i) {
        gameState.platforms[i].entityType = PLATFORM;
        gameState.platforms[i].textureID = platformTextureID;
        gameState.platforms[i].setPosition(glm::vec3(-3 + i - 15, -2.25f, 0));
    }
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        gameState.platforms[i].update(0, NULL, NULL, 0);
    }
    
    
 
}

void processInput() {
    
    gameState.player->setMovement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        // Jump action
                        if (gameState.player ->hitsBottom) {
                            gameState.player->jump = true;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        gameState.player->setMovement(glm::vec3(-1.0f, 0.0f, 0.0f));
        gameState.player->indices = gameState.player->trainerWalkingAnimations[gameState.player->LEFT];
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        gameState.player->setMovement(glm::vec3(1.0f, 0.0f, 0.0f));
        gameState.player->indices = gameState.player->trainerWalkingAnimations[gameState.player->RIGHT];
    }
    

    if (glm::length(gameState.player->getMovement()) > 1.0f) {
        gameState.player->setMovement(glm::normalize(gameState.player->getMovement()));
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void update() {
    
    gameState.player->checkEnemyCollision(gameState.enemies, ENEMY_COUNT);
    if (gameState.player->isAlive == false) {
        success = -1;
        return;
    }
    
    bool anyAlive = false;
    for (int i = 0; i < ENEMY_COUNT; ++i) {
        if (gameState.enemies[i].isAlive == true) {
            anyAlive = true;
            break;
        }
    }
    if (anyAlive == false) {
        success = 1;
        return;
    }
    

    gameState.enemies[1].collisionYCheck(gameState.platforms, PLATFORM_COUNT); // enemies[1] is an gameState object
    if (gameState.enemies[1].hitsBottom && gameState.enemies[1].jump ==false) {
        gameState.enemies[1].jump = true;
    }
    
   float ticks = (float)SDL_GetTicks() / 1000.0f;
   float deltaTime = ticks - lastTicks;
   lastTicks = ticks;
   deltaTime += accumulator;
   if (deltaTime < FIXED_TIMESTEP) {
       accumulator = deltaTime;
       return;
   }
  
   while (deltaTime >= FIXED_TIMESTEP) {
       
       gameState.player->update(FIXED_TIMESTEP, gameState.player,gameState.platforms, PLATFORM_COUNT);
       
       for (int i = 0; i < ENEMY_COUNT; ++i) {
           gameState.enemies[i].update(FIXED_TIMESTEP, gameState.player,gameState.platforms, PLATFORM_COUNT);
       }
       
       deltaTime -= FIXED_TIMESTEP;
   }
   accumulator = deltaTime;
}


void drawText(ShaderProgram *program, GLuint fontTextureID, string text,
              float size, float spacing, glm::vec3 mPosition) {
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    vector<float> vertices;
    vector<float> texCoords;
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
    }
    // end the loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, mPosition);
    program->set_model_matrix(modelMatrix);
    glUseProgram(program->get_program_id());
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


void render() {
    glClear(GL_COLOR_BUFFER_BIT);

//    gameState.map->render(&program);
   
    gameState.player->render(&program);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        gameState.platforms[i].render(&program);
    }
    
    for (int i = 0; i < ENEMY_COUNT; ++i) {
        gameState.enemies[i].render(&program);
    }
    
    
    if (success == -1) {
        drawText(&program, fontTextureID, "You Lose!", 1, -0.5, glm::vec3(-2.0f, 0, 0));
    }
    else if (success == 1) {
        drawText(&program, fontTextureID, "You Win!", 1, -0.5, glm::vec3(-1.785f, 0, 0));
    }
    

    SDL_GL_SwapWindow(displayWindow);
}


void shutdown() {
    Mix_FreeMusic(g_music);
//    delete gameState.map;
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initialize();
    
    while (gameIsRunning) {
        processInput();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
