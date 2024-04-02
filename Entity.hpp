#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"


enum EntityType {PLAYER, PLATFORM, ENEMY};

enum AIType {WALKER, WAITANDGO, JUMPER};
enum AIState {WALKING, IDLE, ATTACKING, JUMPING};

class Entity {
private:
    int* animationRight = NULL;
    int* animationLeft = NULL;
    int* animationUp = NULL;
    int* animationDown = NULL;
    
    glm::vec3 mPosition;
    glm::vec3 mMovement;
    
    
    
public:
    EntityType entityType;
    AIType aiType;
    AIState aiState;
    glm::vec3 mAcceleration;
    glm::vec3 mVelocity;
    
//-------TRANSFORMATIONS-------//
    float mSpeed;
    glm::mat4 mModelMatrix;
    
    
    float width = 1.0f;
    float height = 1.0f;
    
    bool jump = false;
    float jumpPower = 0;
    
    
    GLuint textureID;
    
// ————— STATIC VARIABLES ————— //
    static const int SECONDS_PER_FRAME = 4;
    static const int LEFT  = 0,
                        RIGHT = 1,
                        UP    = 2,
                        DOWN  = 3;
//------ANIMATION-------//
    int **trainerWalkingAnimations = new int* [4]{
        animationLeft,
        animationRight,
        animationUp,
        animationDown
    };

    int frames = 0;
    int index = 0;
    int cols = 0;
    int rows = 0;
    
    int* indices = NULL;
    float animationTime = 0.0f;

    
    bool hitsTop = false;
    bool hitsBottom = false;
    bool hitsLeft = false;
    bool hitsRight = false;
    
    bool pitRight = false;
    bool pitLeft = false;
    
    bool isAlive = true;

    Entity();
    ~Entity();
    
    void drawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
//------GETTERS-------//
    const glm::vec3 getPosition() const ;
    const glm::vec3 getMovement() const;
    
//------SETTERS------//
    void setPosition(glm::vec3 newPosition);
    void setMovement(glm::vec3 newMovement);
    
    bool collisionCheck(Entity* wildPokemon);
    void collisionXCheck(Entity *collidableobjs, int objCount);
    void collisionYCheck(Entity *collidableobjs, int objCount);
    void checkEnemyCollision(Entity *wildPokemons, int pokemonCount);
    
    void  pitAvoidance(Entity *platforms, int platformCount);

    void aiActivate(Entity *poketrainer, Entity *platforms, int platformCount);
    void aiWalk(Entity *platforms, int platformCount);
    void aiGuard(Entity *poketrainer);
    void aiJump();
    
    void update(float deltaTime, Entity *poketrainer, Entity *platforms, int platformCount);
    void render(ShaderProgram* program);
    
    

};
