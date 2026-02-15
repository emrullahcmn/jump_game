#ifndef GAME_FUNCTIONS_H
#define GAME_FUNCTIONS_H

#include "game_structs.h"
#include "game_macros.h"
#include <math.h>
#include <stdio.h>


void obstacleAnimate(Animation *animation){
    animation->framesCounter++;
    if (animation->framesCounter >= (FPS / animation->framesSpeed)){
        animation->framesCounter = 0;
        animation->frameRec.x = animation->frameRecStatic->x + animation->currentFrame * (*animation->frameRecStatic).width;
        animation->currentFrame++;
        animation->currentFrame %= 2;
    }
}

int next(int index){
    return (++index) % ACTIVE_OBSTACLES; 
}

void updateObstaclePositions(ActiveObstacle *activeObstacles, float speed, float frameTime, float *distToRight, int *head, int *tail){
    *distToRight -= speed * frameTime;
    for (int i = *tail; i != *head; i = next(i)){
        activeObstacles[i].pos.x += speed * frameTime;  
    }
    if(activeObstacles[*tail].pos.x <= OUT_OF_SCREEN && *head != *tail){
        *tail = next(*tail);
    }
}

void clearObstaclePositions(ActiveObstacle *activeObstacles, int head, int tail){
    for (int i = tail; i != head; i = next(i)){
        activeObstacles[i].pos.x = OUT_OF_SCREEN;
    }
}

// [0, 17] small obstacles
// [18, 27] big obstacles
void makeANewObstacle(ActiveObstacle *activeObstacles, Obstacle *obstacles, int *latestWidth, int *lastHeight, float *distToRight, int animatedObstacleIx,
    int *head, int *tail, bool levelIsHard){
    int index;
    if(levelIsHard){
        index = GetRandomValue(0, 9);
        if(index == 0){
            index = GetRandomValue(18, 26);
        }else if(index >= 1 && index <= 3){
            index = 28;
        }else{
            index = 27;
        }
    }else{
        index = GetRandomValue(0, 17);
    }

    activeObstacles[*head].pos.x =GAME_SCREEN_WIDTH + obstacles[index].rect.width;//GAME_SCREEN_WIDTH  - obstacles[index].rect.width;   //TODO: ...
    
    if(index >= animatedObstacleIx){
        index = animatedObstacleIx;
        activeObstacles[*head].pos.y = GAME_SCREEN_HEIGHT - BOTTOM_MARGIN - obstacles[index].rect.height - GetRandomValue(0, PLAYER_HEIGHT*2);
    }else{
        activeObstacles[*head].pos.y = GAME_SCREEN_HEIGHT - BOTTOM_MARGIN - obstacles[index].rect.height;
    }
    activeObstacles[*head].index = index;
    *latestWidth = obstacles[index].rect.width;
    *lastHeight = obstacles[index].rect.height;
    *head = next(*head);
    *distToRight = 0;   
}

// obstacles array consists of just static obstacle blueprints, we can active any number of them using activeObstacle
void drawActiveObstacles(ActiveObstacle *activeObstacles, Obstacle *obstacles, Texture2D sprite, int animatedObstacleIx, Animation *animatedObstacle, int gameState,
int head, int tail){
    int index;
    bool animated = false;
    if(activeObstacles[tail].pos.x <= OUT_OF_SCREEN){
        return;
    }
    for (int i = tail; i != head; i = next(i)){
        index = activeObstacles[i].index;
        if(index == animatedObstacleIx){
            if(!animated && gameState == GAME_RUNNING){
                obstacleAnimate(animatedObstacle);
                animated = true;
            }
            DrawTextureRec(sprite, animatedObstacle->frameRec, activeObstacles[i].pos, obstacles[index].color);
        }else{
            DrawTextureRec(sprite, obstacles[index].rect, activeObstacles[i].pos, obstacles[index].color);
        }
    }
}



// tail points to left-most valid obstacle
bool checkCollisionWithObstacles(ActiveObstacle *activeObstacles, Obstacle *obstacles, Rectangle player, bool ducking,
        Collision *playerStandingCollisions, Collision *playerDuckingCollisions,
        Collision *flyingObstacleFirstCollisions,  Collision *flyingObstacleSecondCollisions,
        Animation *animatedObstacle,
        int head, int tail
){
    if(tail == head) return false;

    Vector2 playerCCenter;
    float playerCRadius;
    Vector2 *gamePos;
    Rectangle obstacleRect;
    int index;
    for (int i = tail; i != head; i = next(i)){
        index = activeObstacles[i].index;
        gamePos = &activeObstacles[i].pos;

        if((gamePos->x > player.x && gamePos->x - player.x > player.width)){
            continue;
        }
        if((gamePos->x < player.x && player.x - gamePos->x > obstacles[index].rect.width)){
            continue;
        }

        if(index != FLYING_OBSTACLE_INDEX){
            obstacleRect = (Rectangle){gamePos->x, gamePos->y, obstacles[index].rect.width, obstacles[index].rect.height};
            int length = !ducking ? PLAYER_STANDING_COLLISIONS : PLAYER_DUCKING_COLLISIONS;
            for(int j = 0; j<length; j++){  
                playerCCenter.x = !ducking ? playerStandingCollisions[j].relativePos.x + player.x : playerDuckingCollisions[j].relativePos.x + player.x;
                playerCCenter.y = !ducking ? playerStandingCollisions[j].relativePos.y + player.y : playerDuckingCollisions[j].relativePos.y + player.y;
                playerCRadius = !ducking ? playerStandingCollisions[j].radius : playerDuckingCollisions[j].radius;
                if(CheckCollisionCircleRec(playerCCenter, playerCRadius, obstacleRect)){
                    return true;
                }
            }
            
        }else{
            // flying obstacle stuff here
            Vector2 obstacleCCenter;
            float obstacleCRadius;
            int length = !ducking ? PLAYER_STANDING_COLLISIONS : PLAYER_DUCKING_COLLISIONS;
            for(int j = 0; j<length; j++){  

                playerCCenter.x = !ducking ? playerStandingCollisions[j].relativePos.x + player.x : playerDuckingCollisions[j].relativePos.x + player.x;
                playerCCenter.y = !ducking ? playerStandingCollisions[j].relativePos.y + player.y : playerDuckingCollisions[j].relativePos.y + player.y;
                playerCRadius = !ducking ? playerStandingCollisions[j].radius : playerDuckingCollisions[j].radius;

                for(int k = 0; k<FLYING_OBSTACLE_COLLISIONS; k++){ 
                    obstacleCCenter.x = animatedObstacle->currentFrame ? flyingObstacleFirstCollisions[k].relativePos.x + gamePos->x : flyingObstacleSecondCollisions[k].relativePos.x + gamePos->x;
                    obstacleCCenter.y = animatedObstacle->currentFrame ? flyingObstacleFirstCollisions[k].relativePos.y + gamePos->y : flyingObstacleSecondCollisions[k].relativePos.y + gamePos->y;
                    obstacleCRadius = animatedObstacle->currentFrame ?flyingObstacleFirstCollisions[k].radius : flyingObstacleSecondCollisions[k].radius;
                    if(CheckCollisionCircles(playerCCenter, playerCRadius, obstacleCCenter, obstacleCRadius)){
                        return true;
                    }
                }
            }
            
        }
        
    }
    return false;
}

// for debugging
void drawCollisions(ActiveObstacle *activeObstacles, Obstacle *obstacles, Rectangle player, bool ducking,
        Collision *playerStandingCollisions, Collision *playerDuckingCollisions,
        Collision *flyingObstacleFirstCollisions,  Collision *flyingObstacleSecondCollisions,
        Animation *animatedObstacle, Rectangle *standingUpFrameRec
){
    Vector2 playerCCenter;
    float playerCRadius;
    int length = !ducking ? PLAYER_STANDING_COLLISIONS : PLAYER_DUCKING_COLLISIONS;
    for(int j = 0; j<length; j++){   // TODO: arrange this length please! with ternary. OK both is 6
        playerCCenter.x = !ducking ? playerStandingCollisions[j].relativePos.x + player.x : playerDuckingCollisions[j].relativePos.x + player.x;
        playerCCenter.y = !ducking ? playerStandingCollisions[j].relativePos.y + player.y : playerDuckingCollisions[j].relativePos.y + player.y;
        playerCRadius = !ducking ? playerStandingCollisions[j].radius : playerDuckingCollisions[j].radius;
        DrawCircleV(playerCCenter, playerCRadius, YELLOW);
    }

    Rectangle obstacleRect;
    Vector2 gamePos;
    for (int i = 0; i < ACTIVE_OBSTACLES; i++){
        int index = activeObstacles[i].index;
        Vector2 gamePos = activeObstacles[i].pos;
        if (gamePos.x > OUT_OF_SCREEN){
            if(index != FLYING_OBSTACLE_INDEX){        // not flying
                obstacleRect = (Rectangle){gamePos.x, gamePos.y, obstacles[index].rect.width, obstacles[index].rect.height};
                DrawRectangleRec(obstacleRect, GREEN);
            }else{
                // flying obstacle stuff here
                Vector2 obstacleCCenter;
                float obstacleCRadius;
                for(int k = 0; k<FLYING_OBSTACLE_COLLISIONS; k++){
                    obstacleCCenter.x = animatedObstacle->currentFrame ? flyingObstacleFirstCollisions[k].relativePos.x + gamePos.x : flyingObstacleSecondCollisions[k].relativePos.x + gamePos.x;
                    obstacleCCenter.y = animatedObstacle->currentFrame ? flyingObstacleFirstCollisions[k].relativePos.y + gamePos.y : flyingObstacleSecondCollisions[k].relativePos.y + gamePos.y;
                    obstacleCRadius = animatedObstacle->currentFrame ?flyingObstacleFirstCollisions[k].radius : flyingObstacleSecondCollisions[k].radius;
                    DrawCircleV(obstacleCCenter, obstacleCRadius, PINK);
                }
                
            }
        }
    }
}

float findLandingDistance(float speed, float gravity, float jumpSpeed, float frameTime, int latestWidth, int lastHeight, Level *level, bool levelIsHard){

    if (speed < 0)
        speed = -speed;
    if (jumpSpeed < 0)
        jumpSpeed = -jumpSpeed;
    int t = (int)ceilf(jumpSpeed / gravity);

    int rndval = GetRandomValue(level->start, level->end); 
    if(levelIsHard) t /= 2;

    return (t + FPS * rndval / 100.f) * speed * frameTime + lastHeight;
}

/* void runAnimation(Animation *animation, void (*animationFunc)(Animation *) ){
    animationFunc(animation);
} */

void playerAnimate(Animation *animation, int collisionDetected, Rectangle *player, bool ducking){
    animation->frameRec.y = animation->frameRecStatic->y;
    animation->framesCounter++;
    if (animation->framesCounter >= (FPS / animation->framesSpeed)){
        animation->framesCounter = 0;

        if(!ducking){

            if (animation->currentFrame == 1)
                animation->currentFrame++; // skip frame 1
    
            if (!collisionDetected){ // if on air, fix player to some frame
                animation->currentFrame = 0;
            }
            animation->frameRec.x = animation->frameRecStatic->x + animation->currentFrame * (*player).width;
            animation->currentFrame++;
            if (animation->currentFrame > 3)
                animation->currentFrame = 0;
        }else{
            // ducking
            animation->frameRec.x = animation->frameRecStatic->x + animation->currentFrame * (*player).width;
            animation->currentFrame++;
            if (animation->currentFrame > 1)
                animation->currentFrame = 0;
        }
    }
}



void modifyAnimationInformation(Animation *playerAnimation, Rectangle *player, Rectangle *xingFrameRec){
    playerAnimation->frameRecStatic = xingFrameRec;
    player->width = xingFrameRec->width;
    player->height = xingFrameRec->height;
    playerAnimation->frameRec.width = xingFrameRec->width;
    playerAnimation->frameRec.height = xingFrameRec->height;
    playerAnimation->frameRec.x = xingFrameRec->x;
    playerAnimation->frameRec.y = xingFrameRec->y;

}

void setPlayerDucking(Animation *playerAnimation, Rectangle *player, Rectangle *duckingFrameRec){
    modifyAnimationInformation(playerAnimation, player, duckingFrameRec);
}
// then make ducking = false   // do not call these continously because they initialize freameRec.x and frameRec.y to default! which affects animations
void setPlayerStandingUp(Animation *playerAnimation, Rectangle *player, Rectangle *standingUpFrameRec){
    modifyAnimationInformation(playerAnimation, player, standingUpFrameRec);
}

// obstacle speed float
void roadAnimate(Animation *animation, float obstacleSpeed, float frameTime){
    animation->frameRec.x -= obstacleSpeed * frameTime; // obstacle speed negative so this must be positive, going to right
    if (animation->frameRec.x > 1200){
        animation->frameRec.x -= 1200;
    }
}

#endif