/**
* Author: Andy Ng
* Assignment: Rise of the AI
* Date due: 2024-03-30, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Entity.hpp"
#include <iostream>

using namespace std;

Entity::Entity()
{
    mPosition = glm::vec3(0);
    mMovement = glm::vec3(0);
    mAcceleration = glm::vec3(0);
    mVelocity = glm::vec3(0);
    mSpeed = 0;
    mModelMatrix = glm::mat4(1.0f);
}

Entity::~Entity()
{
    delete [] animationLeft;
    delete [] animationRight;
    delete [] animationUp;
    delete [] animationDown;
    delete [] trainerWalkingAnimations;
}

//------GETTERS-------//

const glm::vec3 Entity::getPosition() const {return mPosition;};
const glm::vec3 Entity::getMovement() const {return mMovement;};

//------SETTERS------//
void Entity::setPosition(glm::vec3 newPosition) {mPosition = newPosition;};
void Entity::setMovement(glm::vec3 newMovement) {mMovement = newMovement;};


bool Entity::collisionCheck(Entity *other) {
    if (isAlive == false || other->isAlive == false){
        return false;
    }
    float xdist = fabs(mPosition.x - other->mPosition.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(mPosition.y - other->mPosition.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0){
        return true;
    }
    return false;
}

void Entity::collisionYCheck(Entity *collidableobjs, int objCount){

    for (int i = 0; i < objCount; i++)
    {
        Entity *object = &collidableobjs[i];
        if (collisionCheck(object))
        {
            float ydist = fabs(mPosition.y - object->mPosition.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (mVelocity.y > 0) {
                mPosition.y -= penetrationY;
                mVelocity.y = 0;
                hitsTop = true;
                object->hitsBottom = true;
            }
            else if (mVelocity.y < 0) {
                mPosition.y += penetrationY;
                mVelocity.y = 0;
                hitsBottom = true;
                object->hitsTop = true;
            }
        }
    }
}
void Entity::collisionXCheck(Entity *collidableobjs, int objCount){
    
    for (int i = 0; i < objCount; i++)
    {
        Entity *object = &collidableobjs[i];
        if (collisionCheck(object))
        {
            float xdist = fabs(mPosition.x - object->mPosition.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (mVelocity.x > 0) {
                mPosition.x -= penetrationX;
                mVelocity.x = 0;
                hitsRight = true;
                object->hitsLeft = true;
            }
            else if (mVelocity.x < 0) {
                mPosition.x += penetrationX;
                mVelocity.x = 0;
                hitsLeft = true;
                object->hitsRight = true;
            }
        }
    }
}
    
            

void Entity::checkEnemyCollision(Entity *wildpokemons, int pokemonCount) {
    for (int i = 0; i < pokemonCount; ++i) {
        if (collisionCheck(&wildpokemons[i])) {
            if (mVelocity.y < 0 && wildpokemons[i].mPosition.y <= mPosition.y) {
                // if velocity of player is negative and wild pokemon is below the player then the wild pokemon dies
                //basically when player jumps on pokemon
                wildpokemons[i].isAlive = false;
            }
            else {
                //if not and player touches enemy then player dies
                isAlive = false;
            }
        }
    }
}

void Entity::pitAvoidance(Entity *platforms, int platformCount) {
    int leftLocation = -1;
    int rightLocation = -1;
    glm::vec3 sensorRight = glm::vec3(mPosition.x + 0.7f, mPosition.y - 0.5f, 0);
    glm::vec3 sensorLeft = glm::vec3(mPosition.x - 0.7f, mPosition.y - 0.5f, 0);
    for (int i = 0; i < platformCount; ++i) {
        float leftDistX = fabs(sensorLeft.x - platforms[i].mPosition.x) - ((width + platforms[i].width) / 2.0f);
        float leftDistY = fabs(sensorLeft.y - platforms[i].mPosition.y) - ((height + platforms[i].height) / 2.0f);
        float rightDistX = fabs(sensorRight.x - platforms[i].mPosition.x) - ((width + platforms[i].width) / 2.0f);
        float rightDistY = fabs(sensorRight.y - platforms[i].mPosition.y) - ((height + platforms[i].height) / 2.0f);
        
        if (leftDistX < 0 && leftDistY < 0){
            leftLocation = 0;
        }
        if (rightDistX < 0 && rightDistY < 0){
            rightLocation = 0;
        }
    }
    if (leftLocation == -1) pitLeft = true;
    if (rightLocation == -1) pitRight = true;
}

void Entity::aiActivate(Entity *player, Entity *platforms, int platformCount) {
    switch(aiType) {
        case WALKER:
            aiWalk(platforms, platformCount);
            break;
        case WAITANDGO:
            aiGuard(player);
            break;
        case JUMPER:
            aiJump();
            break;
    }
}

void Entity::aiJump() {
    if (jump) {
        jump = false;
        mVelocity.y += jumpPower;
    }
}

void Entity::aiWalk(Entity *platforms, int platformCount) {
    if (mMovement == glm::vec3(0)) {
        mMovement = glm::vec3(-1, 0, 0);
    }
    pitAvoidance(platforms, platformCount);
    if (pitLeft) {
        mMovement.x = 1;
        pitLeft = false;
    }
    if (pitRight) {
        mMovement.x = -1;
        pitRight = false;
    }
}

void Entity::aiGuard(Entity *player) {
    switch(aiState) {
        case IDLE:
            if (fabs(mPosition.x - player->mPosition.x) < 2.0f) {
                aiState = WALKING;
            }
            break;
            
        case WALKING:
            if (mPosition.x > player->mPosition.x) {
                mMovement = glm::vec3(-1, 0, 0);
            }
            else if (mPosition.x < player->mPosition.x) {
                mMovement = glm::vec3(1, 0, 0);
            }
            break;
    
        case JUMPING:
            break;
            
        case ATTACKING:
            break;
    }
}

void Entity::update(float deltaTime, Entity *player, Entity *platforms, int platformCount)
{
    if (isAlive == false) {// if entity is dead no need to update
        return;
    }
    hitsTop = false;
    hitsBottom = false;
    hitsLeft = false;
    hitsRight = false;
    
    pitLeft = false;
    pitRight = false;
    
    if (entityType == ENEMY) {
         aiActivate(player, platforms, platformCount);
    }
    
    if (indices != NULL) {
        if (glm::length(mMovement) != 0) {
            animationTime += deltaTime;
            if (animationTime >= 0.25f) {
                animationTime = 0.0f;
                index++;
                if (index >= frames) {
                    index = 0;
                }
            }
        } 
        else {
            index = 0;
        }
    }
    if (jump) {
        jump = false;
        mVelocity.y += jumpPower;
    }
    
    mVelocity.x = mMovement.x * mSpeed;

    mVelocity += mAcceleration * deltaTime;
    
    mPosition.y += mVelocity.y * deltaTime;
    collisionYCheck(platforms, platformCount);
    
    mPosition.x += mVelocity.x * deltaTime;
    collisionXCheck(platforms, platformCount);
    
    mModelMatrix = glm::mat4(1.0f);
    mModelMatrix = glm::translate(mModelMatrix, mPosition);
}

                               

void Entity::drawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % cols) / (float)cols;
    float v = (float)(index / cols) / (float)rows;
    
    float width = 1.0f / (float)cols;
    float height = 1.0f / (float)rows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::render(ShaderProgram *program) {
    if (isAlive == false) {
        return;
    }
    
    program->set_model_matrix(mModelMatrix);
    
    if (indices != NULL) { // if indices is not null then use sprite from texture function
        drawSpriteFromTextureAtlas(program, textureID, indices[index]);
        return;
    }
    //
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    //
}
